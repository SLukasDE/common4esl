#include <common4esl/config/Config.h>
#include <common4esl/config/FilePosition.h>
#include <common4esl/Logger.h>

#include <cstdlib>


namespace common4esl {
namespace config {

namespace {
Logger logger("common4esl::config::Config");
} /* anonymous namespace */

Config::Config(const std::string& aFileName)
: fileName(aFileName),
  lineNo(-1)
{ }

Config::Config(const std::string& aFileName, const tinyxml2::XMLElement& aElement)
: fileName(aFileName),
  lineNo(aElement.GetLineNum())
{ }

std::string Config::evaluate(const std::string& expression, const std::string& language) const {
	if(language == "plain") {
		return expression;
	}
	else if(language == "") { // || language == "env-var") {
		std::string value;
		std::string var;
		std::string tmpValue;
		enum {
			intro,
			begin,
			end
		} state = end;

		for(std::size_t i=0; i<expression.size(); ++i) {
			if(state == begin) {
				if(expression.at(i) == '}') {
					char* val = getenv(var.c_str());
					if(val == nullptr) {
						throw FilePosition::add(*this, "No value available for variable \"" + var + "\" in expression: \"" + expression + "\"");
					}
					value += val;
					state = end;
					var.clear();
					tmpValue.clear();
				}
				else {
					var += expression.at(i);
					tmpValue = expression.at(i);
				}
			}
			else if(state == intro) {
				if(expression.at(i) == '{') {
					state = begin;
				}
				else {
					//throw FilePosition::add(*this, "Syntax error in expression: \"" + expression + "\"");

					state = end;

					value += tmpValue;
					tmpValue.clear();

					value += expression.at(i);
				}
			}
			else {
				if(expression.at(i) == '$') {
					state = intro;
					tmpValue = expression.at(i);
				}
				else {
					value += expression.at(i);
				}
			}
		}

		if(state != end) {
			value += tmpValue;
		}

		return value;
	}

	throw FilePosition::add(*this, "Syntax error in expression. Unknown language \"" + language + "\"");
}

const std::string& Config::getFileName() const noexcept {
	return fileName;
}

int Config::getLineNo() const noexcept {
	return lineNo;
}

std::pair<std::string, int> Config::getXMLFile() const noexcept {
	return std::pair<std::string, int>(fileName, lineNo);
}

std::pair<std::string, int> Config::setXMLFile(const std::string& aFileName, int aLineNo) {
	std::pair<std::string, int> oldXmlFile(fileName, lineNo);

	fileName = aFileName;
	lineNo = aLineNo;

	return oldXmlFile;
}

std::pair<std::string, int> Config::setXMLFile(const std::string& aFileName, const tinyxml2::XMLElement& aElement) {
	std::pair<std::string, int> oldXmlFile(fileName, lineNo);

	fileName = aFileName;
	lineNo = aElement.GetLineNum();

	return oldXmlFile;
}

std::pair<std::string, int> Config::setXMLFile(const std::pair<std::string, int>& aXmlFile) {
	std::pair<std::string, int> oldXmlFile(fileName, lineNo);

	fileName = aXmlFile.first;
	lineNo = aXmlFile.second;

	return oldXmlFile;
}

std::string Config::makeSpaces(std::size_t spaces) const {
	std::string rv;
	for(std::size_t i=0; i<spaces; ++i) {
		rv += " ";
	}
	return rv;
}

bool Config::stringToBool(bool& b, std::string str) {
	if(str == "true") {
		b = true;
	}
	else if(str == "false") {
		b = false;
	}
	else {
		return false;
	}
	return true;
}

} /* namespace config */
} /* namespace common4esl */
