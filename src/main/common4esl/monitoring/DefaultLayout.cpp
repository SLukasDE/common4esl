#include <common4esl/monitoring/DefaultLayout.h>

#include <esl/utility/String.h>

#include <esl/monitoring/Logging.h>
#include <esl/plugin/Registry.h>
#include <esl/system/Stacktrace.h>

#include <chrono>
#include <ctime>
#include <functional>
#include <stdexcept>
#include <time.h>

namespace common4esl {
inline namespace v1_6 {
namespace monitoring {

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
#ifdef _WIN32
    localtime_s(&timeBuf, &timestamp);
    timePtr = &timeBuf;
#else
    timePtr = localtime_r(&timestamp, &timeBuf);
#endif
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

std::string formatLevel(esl::monitoring::Streams::Level level) {
    switch(level) {
    case esl::monitoring::Streams::Level::trace:
    	return "[TRACE] ";
    case esl::monitoring::Streams::Level::debug:
    	return "[DEBUG] ";
    case esl::monitoring::Streams::Level::info:
    	return "[INFO ] ";
    case esl::monitoring::Streams::Level::warn:
    	return"[WARN ] ";
    case esl::monitoring::Streams::Level::error:
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

DefaultLayout::DefaultLayout(const esl::monitoring::SimpleLayout::Settings& aSettings)
: settings(aSettings)
{ }

std::string DefaultLayout::toString(const esl::monitoring::Streams::Location& location) const {
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
		if(esl::plugin::Registry::get().findObject<esl::monitoring::Logging>()) {
			rv += "-" + formatThreadNo(esl::plugin::Registry::get().findObject<esl::monitoring::Logging>()->getThreadNo(location.threadId));
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

} /* namespace monitoring */
} /* inline namespace v1_6 */
} /* namespace common4esl */
