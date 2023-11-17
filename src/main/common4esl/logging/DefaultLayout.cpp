#include <common4esl/logging/DefaultLayout.h>

#include <esl/utility/String.h>

#include <esl/logging/Logging.h>
#include <esl/plugin/Registry.h>
#include <esl/system/Stacktrace.h>

#include <chrono>
#include <ctime>
#include <functional>
#include <stdexcept>
#include <time.h>

namespace common4esl {
inline namespace v1_6 {
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

std::string formatTimestamp(const std::chrono::time_point<std::chrono::system_clock>& time_point) {
	auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch());
	std::time_t timestamp = millisecs.count() / 1000;
	return formatTimestamp(timestamp);
}

std::string formatLevel(esl::logging::Streams::Level level) {
    switch(level) {
    case esl::logging::Streams::Level::TRACE:
    	return "[TRACE] ";
    case esl::logging::Streams::Level::DEBUG:
    	return "[DEBUG] ";
    case esl::logging::Streams::Level::INFO:
    	return "[INFO ] ";
    case esl::logging::Streams::Level::WARN:
    	return"[WARN ] ";
    case esl::logging::Streams::Level::ERROR:
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

} /* anonymous namespace */

DefaultLayout::DefaultLayout(const esl::logging::SimpleLayout::Settings& aSettings)
: settings(aSettings)
{ }

std::string DefaultLayout::toString(const esl::logging::Streams::Location& location) const {
	std::string rv;

	if(settings.showTimestamp) {
		rv += formatTimestamp(location.timestamp);
	}

	if(settings.showLevel) {
		rv += formatLevel(location.level);
	}

    rv += "(";
	if(settings.showTypename) {
	    rv += formatTypeName(location.typeName);
	}
	if(settings.showThreadNo) {
		if(esl::plugin::Registry::get().findObject<esl::logging::Logging>()) {
			rv += "-" + formatThreadNo(esl::plugin::Registry::get().findObject<esl::logging::Logging>()->getThreadNo(location.threadId));
		}
		else {
			rv += "- (thread?)";
		}
	}
	if(settings.showAddress) {
		rv += " @ " + formatObject(location.object);
	}
	if(settings.showFunction) {
		rv += "|" + formatStrToSize(makeString(location.function), false, 20);
	}
	if(settings.showFile) {
		rv += "|" + formatStrToSize(makeString(location.file), false, 20);
	}
	if(settings.showLineNo) {
		rv += "|" + formatLineNo(location.line);
	}
	rv += "): ";

	return rv;
}

} /* namespace logging */
} /* inline namespace v1_6 */
} /* namespace common4esl */
