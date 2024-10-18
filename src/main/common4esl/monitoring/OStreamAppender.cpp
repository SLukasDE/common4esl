#include <common4esl/monitoring/OStreamAppender.h>

#include <esl/monitoring/Logger.h>

#include <cstdio>
#include <iostream>

namespace common4esl {
inline namespace v1_6 {
namespace monitoring {

namespace {
bool equal(const esl::monitoring::Streams::Location& location1, const esl::monitoring::Streams::Location& location2) {
	return location1.level == location2.level &&
			location1.object == location2.object &&
			location1.typeName == location2.typeName &&
			location1.function == location2.function &&
			location1.file == location2.file &&
			location1.threadId == location2.threadId;
}
} /* anonymous namespce */

OStreamAppender::OStreamAppender(const esl::monitoring::OStreamAppender::Settings& aSettings)
: settings(aSettings)
{ }

void OStreamAppender::setLayout(const esl::monitoring::Layout* aLayout) {
	layout = aLayout;
}

const esl::monitoring::Layout* OStreamAppender::getLayout() const {
	return layout;
}

/* both methods are NOT thread-safe */
void OStreamAppender::setRecordLevel(esl::monitoring::Appender::RecordLevel aRecordLevel) {
	recordLevel = aRecordLevel;
}

esl::monitoring::Appender::RecordLevel OStreamAppender::getRecordLevel() const {
	return recordLevel;
}

void OStreamAppender::flush(std::ostream*) {
	switch(getRecordLevel()) {
	case RecordLevel::OFF:
		return;
	case RecordLevel::ALL:
		break;
	default: /* RecordLevel::SELECTED */
		if(!lastLocation.enabled) {
			return;
		}
		break;
	}

	getOStream(lastLocation.level).flush();
}

void OStreamAppender::write(const esl::monitoring::Streams::Location& aLocation, const char* ptr, std::size_t size) {
	switch(getRecordLevel()) {
	case RecordLevel::OFF:
		return;
	case RecordLevel::ALL:
		break;
	default: /* RecordLevel::SELECTED */
		if(!aLocation.enabled) {
			return;
		}
		break;
	}

	std::ostream& oStream = getOStream(aLocation.level);

	if(!equal(lastLocation, aLocation)) {
		if(!isFirstCharacterInLine) {
			std::ostream& lastOStream = getOStream(lastLocation.level);

			lastOStream << "\n";
			if(&lastOStream != &oStream) {
				lastOStream.flush();
			}

			isFirstCharacterInLine = true;
		}
		lastLocation = aLocation;
	}

	const char* begin = ptr;

	for(auto iter = ptr; iter != &ptr[size]; ++iter) {
		if(isFirstCharacterInLine) {
			if(getLayout()) {
				oStream << getLayout()->toString(aLocation);
			}
			isFirstCharacterInLine = false;
		}

		if(*iter == '\n') {
			oStream << std::string(begin, iter) << "\n";
			isFirstCharacterInLine = true;
			begin = iter+1;
		}
	}
	oStream << std::string(begin, &ptr[size]);
}

std::ostream& OStreamAppender::getOStream(esl::monitoring::Streams::Level level) {
	switch(level) {
	case esl::monitoring::Streams::Level::trace:
		return settings.trace;
	case esl::monitoring::Streams::Level::debug:
		return settings.debug;
	case esl::monitoring::Streams::Level::info:
		return settings.info;
	case esl::monitoring::Streams::Level::warn:
		return settings.warn;
	case esl::monitoring::Streams::Level::error:
		return settings.error;
	default:
		break;
	}

	return settings.error;
}

} /* namespace monitoring */
} /* inline namespace v1_6 */
} /* namespace common4esl */
