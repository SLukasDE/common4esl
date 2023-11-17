#include <common4esl/config/context/Context.h>
#include <common4esl/config/FilePosition.h>
#include <common4esl/config/context/EntryImpl.h>

#include <esl/Logger.h>


#include <esl/plugin/Registry.h>
#include <esl/plugin/exception/PluginNotFound.h>
#include <esl/object/ProcessingContext.h>
//#include <esl/logging/Logger.h>

#include <iostream>
#include <stdexcept>

namespace common4esl {
inline namespace v1_6 {
namespace config {
namespace context {

namespace {
esl::Logger logger("common4esl::config::context::Context");
} /* anonymous namespace */

Context::Context(const std::string& fileName, const tinyxml2::XMLElement& element)
: Config(fileName, element),
  isRoot(false)
{
	if(element.GetUserData() != nullptr) {
		throw FilePosition::add(*this, "Element has user data but it should be empty");
	}

	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		std::string attributeName = attribute->Name();

		if(attributeName == "id") {
			if(!id.empty()) {
				throw FilePosition::add(*this, "Multiple definitions of attribute 'id'.");
			}
			id = attribute->Value();
			if(id.empty()) {
				throw FilePosition::add(*this, "Invalid value \"\" for attribute 'id'");
			}
		}
		else if(attributeName == "implementation") {
			if(!implementation.empty()) {
				throw std::runtime_error("Multiple definition of attribute 'implementation'.");
			}
			implementation = attribute->Value();
			if(implementation.empty()) {
				throw FilePosition::add(*this, "Invalid value \"\" for attribute 'implementation'");
			}
			if(!refId.empty()) {
				throw FilePosition::add(*this, "Attribute 'implementation' is not allowed together with attribute 'ref-id'.");
			}
		}
		else if(attributeName == "ref-id") {
			if(!refId.empty()) {
				throw FilePosition::add(*this, "Multiple definition of attribute 'ref-id'");
			}
			refId = attribute->Value();
			if(refId.empty()) {
				throw FilePosition::add(*this, "Invalid value \"\" for attribute 'ref-id'");
			}
			if(!implementation.empty()) {
				throw FilePosition::add(*this, "Attribute 'ref-id' is not allowed together with attribute 'implementation'.");
			}
		}
		else {
			throw FilePosition::add(*this, "Unknown attribute '" + attributeName + "'");
		}
	}

	if(refId.empty() && implementation.empty()) {
		throw FilePosition::add(*this, "Attribute 'implementation' is missing.");
	}

	for(const tinyxml2::XMLNode* node = element.FirstChild(); node != nullptr; node = node->NextSibling()) {
		const tinyxml2::XMLElement* innerElement = node->ToElement();

		if(innerElement == nullptr) {
			continue;
		}

		auto oldXmlFile = setXMLFile(getFileName(), *innerElement);
		if(!refId.empty()) {
			throw FilePosition::add(*this, "Definition of elements is not allowed together with attribute 'ref-id'.");
		}

		parseInnerElement(*innerElement);
		setXMLFile(oldXmlFile);
	}
}

Context::Context(const std::string& configuration)
: config::Config("{mem}"),
  isRoot(true)
{
	tinyxml2::XMLError xmlError = xmlDocument.Parse(configuration.c_str(), configuration.size());
	if(xmlError != tinyxml2::XML_SUCCESS) {
		throw FilePosition::add(*this, xmlError);
	}

	const tinyxml2::XMLElement* element = xmlDocument.RootElement();
	if(element == nullptr) {
		throw FilePosition::add(*this, "No root element");
	}

	setXMLFile(getFileName(), *element);
	loadXML(*element);
}

Context::Context(const boost::filesystem::path& filename)
: config::Config(filename.generic_string()),
  isRoot(true)
{
	filesLoaded.insert(filename.generic_string());

	tinyxml2::XMLError xmlError = xmlDocument.LoadFile(filename.generic_string().c_str());
	if(xmlError != tinyxml2::XML_SUCCESS) {
		throw FilePosition::add(*this, xmlError);
	}

	const tinyxml2::XMLElement* element = xmlDocument.RootElement();
	if(element == nullptr) {
		throw FilePosition::add(*this, "No root element");
	}

	setXMLFile(filename.generic_string(), *element);
	loadXML(*element);
}

void Context::save(std::ostream& oStream, std::size_t spaces) const {
	if(isRoot) {
		oStream << "\n<esl-context>\n";

		for(const auto& entry : libraries) {
			if(entry.second.empty()) {
				oStream << "  <library file=\"" << entry.first << "\"/>\n";
			}
			else {
				oStream << "  <library file=\"" << entry.first << "\" arguments=\"" << entry.second << "\"/>\n";
			}
		}
		for(const auto& entry : entries) {
			entry->save(oStream, 2);
		}

		oStream << "</esl-context>\n";
	}
	else {
		oStream << makeSpaces(spaces) << "<context";

		if(!id.empty()) {
			oStream << " id=\"" << id << "\"";
		}

		if(!implementation.empty()) {
			oStream << " implementation=\"" << implementation << "\"";
		}

		if(!refId.empty()) {
			oStream << " ref-id=\"" << refId << "\"";
		}

		if(settings.empty()) {
			oStream << "/>\n";
		}
		else {
			oStream << ">\n";

			for(const auto& setting : settings) {
				setting.saveParameter(oStream, spaces+2);
			}

			oStream << makeSpaces(spaces) << "</context>\n";
		}
	}
}

void Context::install(object::ProcessingContext& context) {
	if(isRoot) {
		for(const auto& entry : entries) {
			entry->install(context);
		}
	}
	else {
		if(refId.empty()) {
			context.addObject(id, create());
		}
		else {
			context.addAlias(id, refId);
		}
	}
}

void Context::loadLibraries() {
	/* ************************
	 * load and add libraries *
	 * ********************** */
	for(auto& library : libraries) {
		esl::plugin::Registry::get().loadPlugin(library.first, library.second.c_str());
	}
}

std::unique_ptr<esl::object::Object> Context::create() const {
	std::vector<std::pair<std::string, std::string>> eslSettings;
	for(const auto& setting : settings) {
		eslSettings.push_back(std::make_pair(setting.key, evaluate(setting.value, setting.language)));
	}

	std::unique_ptr<esl::object::ProcessingContext> bootContext;
	try {
		bootContext = esl::plugin::Registry::get().create<esl::object::ProcessingContext>(implementation, eslSettings);
	}
	catch(const esl::plugin::exception::PluginNotFound& e) {
		throw FilePosition::add(*this, e);
	}
	catch(const std::runtime_error& e) {
		throw FilePosition::add(*this, e);
	}
	catch(const std::exception& e) {
		throw FilePosition::add(*this, e);
	}
	catch(...) {
		throw FilePosition::add(*this, "Could not create a boot context with id '" + id + "' for implementation '" + implementation + "' because an unknown exception occurred.");
	}

	if(!bootContext) {
		throw FilePosition::add(*this, "Could not create a boot context with id '" + id + "' for implementation '" + implementation + "'");
	}

	return std::unique_ptr<esl::object::Object>(bootContext.release());
}

void Context::parseInnerElement(const tinyxml2::XMLElement& element) {
	if(element.Name() == nullptr) {
		throw FilePosition::add(*this, "Element name is empty");
	}

	const std::string elementName(element.Name());

	if(elementName == "include" && isRoot == true) {
		parseInclude(element);
	}
	else if(elementName == "library" && isRoot == true) {
		parseLibrary(element);
	}
	/*
	else if(elementName == "esl-logger" && isRoot == true) {
		eslLoggers.push_back(logging::Logger(getFileName(), element));
	}
	*/
	else if(elementName == "parameter" && isRoot == false) {
		settings.push_back(Setting(getFileName(), element, true));
	}
	else {
		entries.emplace_back(new EntryImpl(getFileName(), element));
	}
}

void Context::loadXML(const tinyxml2::XMLElement& element) {
	if(element.Name() == nullptr) {
		throw FilePosition::add(*this, "Name of XML root element is empty");
	}

	const std::string elementName(element.Name());

	if(elementName == "jboot") {
		logger.warn << "Tag 'jboot' is deprecated. Use 'esl-context' instead.\n";
	}
	else if(elementName != "esl-context") {
		throw FilePosition::add(*this, "Name of XML root element is \"" + std::string(element.Name()) + "\" but should be \"esl-context\"");
	}

	if(element.GetUserData() != nullptr) {
		throw FilePosition::add(*this, "Node has user data but it should be empty");
	}

	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		throw FilePosition::add(*this, "Unknown attribute '" + std::string(attribute->Name()) + "'");
	}

	for(const tinyxml2::XMLNode* node = element.FirstChild(); node != nullptr; node = node->NextSibling()) {
		const tinyxml2::XMLElement* innerElement = node->ToElement();

		if(innerElement == nullptr) {
			continue;
		}

		auto oldXmlFile = setXMLFile(getFileName(), *innerElement);
		parseInnerElement(*innerElement);
		setXMLFile(oldXmlFile);
	}
}

void Context::parseInclude(const tinyxml2::XMLElement& element) {
	std::string fileName;

	if(element.GetUserData() != nullptr) {
		throw FilePosition::add(*this, "Element has user data but it should be empty");
	}

	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		std::string attributeName = attribute->Name();

		if(attributeName == "file") {
			fileName = attribute->Value();
			if(fileName.empty()) {
				throw FilePosition::add(*this, "Value \"\" of attribute 'file' is invalid.");
			}
		}
		else {
			throw FilePosition::add(*this, "Unknown attribute '" + attributeName + "'");
		}
	}

	if(fileName.empty()) {
		throw FilePosition::add(*this, "Missing attribute 'file'");
	}

	if(filesLoaded.count(fileName) == 0) {
		auto oldXmlFile = setXMLFile(fileName, -1);
		filesLoaded.insert(fileName);

		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError xmlError = doc.LoadFile(fileName.c_str());
		if(xmlError != tinyxml2::XML_SUCCESS) {
			throw FilePosition::add(*this, xmlError);
		}

		const tinyxml2::XMLElement* element = doc.RootElement();
		if(element == nullptr) {
			throw FilePosition::add(*this, "No root element");
		}

		setXMLFile(fileName, *element);
		loadXML(*element);
		setXMLFile(oldXmlFile);
	}
}

void Context::parseLibrary(const tinyxml2::XMLElement& element) {
	std::string fileName;
	std::string arguments;
	bool hasArguments = false;

	if(element.GetUserData() != nullptr) {
		throw FilePosition::add(*this, "Element has user data but it should be empty");
	}

	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		if(std::string(attribute->Name()) == "file") {
			if(!fileName.empty()) {
				throw FilePosition::add(*this, "Multiple definition of attribute \"file\".");
			}
			fileName = attribute->Value();
			if(fileName.empty()) {
				throw FilePosition::add(*this, "Value \"\" of attribute 'file' is invalid.");
			}
		}
		else if(std::string(attribute->Name()) == "arguments") {
			if(hasArguments) {
				throw FilePosition::add(*this, "Multiple definition of attribute \"arguments\".");
			}
			arguments = attribute->Value();
			hasArguments = true;
		}
		else {
			throw FilePosition::add(*this, "Unknown attribute '" + std::string(attribute->Name()) + "'");
		}
	}

	if(fileName.empty()) {
		throw FilePosition::add(*this, "Missing attribute 'file'");
	}

	libraries.push_back(std::make_pair(fileName, arguments));
}

} /* namespace context */
} /* namespace config */
} /* inline namespace v1_6 */
} /* namespace common4esl */
