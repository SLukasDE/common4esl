#include <common4esl/config/Setting.h>
#include <common4esl/config/FilePosition.h>

namespace common4esl {
namespace config {

Setting::Setting(const std::string& fileName, const tinyxml2::XMLElement& element, bool allowLanguage)
: Config(fileName, element)
{
	if(element.GetUserData() != nullptr) {
		throw FilePosition::add(*this, "Element has user data but it should be empty");
	}

	bool hasValue = false;
	bool hasLanguage = false;
	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		if(std::string(attribute->Name()) == "key") {
			if(!key.empty()) {
				throw FilePosition::add(*this, "Multiple definition of attribute 'key'.");
			}
			key = attribute->Value();
			if(key.empty()) {
				throw FilePosition::add(*this, "Value \"\" of attribute 'key' is invalid.");
			}
		}
		else if(std::string(attribute->Name()) == "value") {
			if(hasValue) {
				throw FilePosition::add(*this, "Multiple definition of attribute 'value'.");
			}
			value = attribute->Value();
			hasValue = true;
		}
		else if(std::string(attribute->Name()) == "language" && allowLanguage) {
			if(hasLanguage) {
				throw FilePosition::add(*this, "Multiple definition of attribute 'language'.");
			}
			language = attribute->Value();
			hasLanguage = true;
		}
		else {
			throw FilePosition::add(*this, "Unknown attribute '" + std::string(attribute->Name()) + "'");
		}
	}

	if(key.empty()) {
		throw FilePosition::add(*this, "Missing attribute 'key'");
	}
	if(!hasValue) {
		throw FilePosition::add(*this, "Missing attribute 'value'");
	}
}

void Setting::saveParameter(std::ostream& oStream, std::size_t spaces) const {
	oStream << makeSpaces(spaces) << "<parameter key=\"" << key << "\" value=\"" << value << "\" language=\"" << language << "\"/>\n";
}

} /* namespace config */
} /* namespace common4esl */
