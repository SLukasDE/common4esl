#include <common4esl/logging/DefaultLayout.h>

#include <esl/logging/Logging.h>
#include <esl/logging/Level.h>
#include <esl/system/Stacktrace.h>
#include <esl/utility/String.h>

#include <functional>
#include <time.h>
#include <ctime>
#include <stdexcept>

namespace common4esl {
namespace logging {

namespace {

std::string formatStrToSize(std::string str, bool spacesAtLeftSide = false, const std::size_t strSize = 50) {
	if(str.size() > strSize) {
		str = "..." + str.substr(str.size()+3-strSize);
	}

	if(spacesAtLeftSide) {
		while(str.size() < strSize) {
			str = " " + str;
		}
	}
	else {
		while(str.size() < strSize) {
			str += " ";
		}
	}

	return str;
}

std::string makeString(const char* typeName) {
	return typeName == nullptr ? "" : typeName;
}

std::string formatTimestamp(const std::time_t& timestamp) {
    char timeStr[23];
    struct tm timeBuf;
    struct tm* timePtr;

    timePtr = localtime_r(&timestamp, &timeBuf);
    sprintf(timeStr, "$ %04d-%02d-%02d %02d:%02d:%02d ",
            timePtr->tm_year + 1900,
            timePtr->tm_mon  + 1,
            timePtr->tm_mday,
            timePtr->tm_hour,
            timePtr->tm_min,
            timePtr->tm_sec);
    return timeStr;
}

std::string formatLevel(esl::logging::Level level) {
    switch(level) {
    case esl::logging::Level::TRACE:
    	return "[TRACE] ";
    case esl::logging::Level::DEBUG:
    	return "[DEBUG] ";
    case esl::logging::Level::INFO:
    	return "[INFO ] ";
    case esl::logging::Level::WARN:
    	return"[WARN ] ";
    case esl::logging::Level::ERROR:
    	return "[ERROR] ";
    default:
        break;
    }
	return "[ n/a ] ";
}

std::string formatTypeName(const char* typeName) {
	return formatStrToSize(makeString(typeName), false, 20);
}

std::string formatThreadNo(unsigned int threadNo) {
	return formatStrToSize(std::to_string(threadNo),true, 3);
}

std::string formatObject(const void* object) {
	char buffer[20];
	std::snprintf(buffer, 20, "%p", object);
	return formatStrToSize(buffer, false, 18);
}

std::string formatLineNo(unsigned int lineNo) {
	return formatStrToSize(std::to_string(lineNo),true, 6);
}

bool convertValueToBool(const std::string& value) {
	std::string normalized = esl::utility::String::toUpper(esl::utility::String::trim(value));
	return (normalized != "" && normalized != "0" && normalized != "NO" && normalized != "FALSE");
}

} /* anonymous namespace */

std::unique_ptr<esl::logging::Layout> DefaultLayout::create(const std::vector<std::pair<std::string, std::string>>& values) {
	return std::unique_ptr<esl::logging::Layout>(new DefaultLayout(values));
}

DefaultLayout::DefaultLayout(const std::vector<std::pair<std::string, std::string>>& settings) {
	bool hasShowTimestamp = false;
	bool hasShowLevel = false;
	bool hasShowTypename = false;
	bool hasShowAddress = false;
	bool hasShowFile = false;
	bool hasShowFunction = false;
	bool hasShowLineNo = false;
	bool hasShowThreadNo = false;


	for(const auto& setting : settings) {
		if(setting.first == "show-timestamp") {
			if(hasShowTimestamp) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-timestamp'."));
			}
			hasShowTimestamp = true;
			showTimestamp = convertValueToBool(setting.second);
		}
		else if(setting.first == "show-level") {
			if(hasShowLevel) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-level'."));
			}
			hasShowLevel = true;
			showLevel = convertValueToBool(setting.second);
		}
		else if(setting.first == "show-typename") {
			if(hasShowTypename) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-typename'."));
			}
			hasShowTypename = true;
			showTypename = convertValueToBool(setting.second);
		}
		else if(setting.first == "show-address") {
			if(hasShowAddress) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-address'."));
			}
			hasShowAddress = true;
			showAddress = convertValueToBool(setting.second);
		}
		else if(setting.first == "show-file") {
			if(hasShowFile) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-file'."));
			}
			hasShowFile = true;
			showFile = convertValueToBool(setting.second);
		}
		else if(setting.first == "show-function") {
			if(hasShowFunction) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-function'."));
			}
			hasShowFunction = true;
			showFunction = convertValueToBool(setting.second);
		}
		else if(setting.first == "show-line-no") {
			if(hasShowLineNo) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-line-no'."));
			}
			hasShowLineNo = true;
			showLineNo = convertValueToBool(setting.second);
		}
		else if(setting.first == "show-thread-no") {
			if(hasShowThreadNo) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-thread-no'."));
			}
			hasShowThreadNo = true;
			showThreadNo = convertValueToBool(setting.second);
		}
		else {
			throw std::runtime_error("Unknown key \"" + setting.first + "\"");
		}
	}
}

std::string DefaultLayout::toString(const esl::logging::Location& location) const {
	std::string rv;

	if(showTimestamp) {
		rv += formatTimestamp(location.timestamp);
	}

	if(showLevel) {
		rv += formatLevel(location.level);
	}

    rv += "(";
	if(showTypename) {
	    rv += formatTypeName(location.typeName);
	}
	if(showThreadNo) {
		if(esl::logging::Logging::get()) {
			rv += "-" + formatThreadNo(esl::logging::Logging::get()->getThreadNo(location.threadId));
		}
		else {
			rv += "- (thread?)";
		}
	}
	if(showAddress) {
		rv += " @ " + formatObject(location.object);
	}
	if(showFunction) {
		rv += "|" + formatStrToSize(makeString(location.function), false, 20);
	}
	if(showFile) {
		rv += "|" + formatStrToSize(makeString(location.file), false, 20);
	}
	if(showLineNo) {
		rv += "|" + formatLineNo(location.line);
	}
	rv += "): ";

	return rv;
}

bool DefaultLayout::getShowTimestamp() const {
	return showTimestamp;
}

bool DefaultLayout::getShowLevel() const {
	return showLevel;
}

bool DefaultLayout::getShowTypename() const {
	return showTypename;
}

bool DefaultLayout::getShowAddress() const {
	return showAddress;
}

bool DefaultLayout::getShowFile() const {
	return showFile;
}

bool DefaultLayout::getShowFunction() const {
	return showFunction;
}

bool DefaultLayout::getShowLineNo() const {
	return showLineNo;
}

bool DefaultLayout::getShowThreadNo() const {
	return showThreadNo;
}

} /* namespace logging */
} /* namespace common4esl */
