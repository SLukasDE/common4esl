#include <common4esl/logging/OStreamAppender.h>
#include <esl/logging/Logger.h>

#include <iostream>
#include <cstdio>

namespace common4esl {
namespace logging {

namespace {
OStreamAppender::OStreams getOStreams(const std::vector<std::pair<std::string, std::string>>& settings) {
	std::ostream* errorStream = nullptr;
	std::ostream* warnStream = nullptr;
	std::ostream* infoStream = nullptr;
	std::ostream* debugStream = nullptr;
	std::ostream* traceStream = nullptr;

	for(auto const& setting : settings) {
		if(setting.first == "error") {
			if(errorStream) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				errorStream = &std::cout;
			}
			else if(setting.second == "err") {
				errorStream = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else if(setting.first == "warn") {
			if(errorStream) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				warnStream = &std::cout;
			}
			else if(setting.second == "err") {
				warnStream = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else if(setting.first == "info") {
			if(errorStream) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				infoStream = &std::cout;
			}
			else if(setting.second == "err") {
				infoStream = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else if(setting.first == "debug") {
			if(errorStream) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				debugStream = &std::cout;
			}
			else if(setting.second == "err") {
				debugStream = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else if(setting.first == "trace") {
			if(errorStream) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				traceStream = &std::cout;
			}
			else if(setting.second == "err") {
				traceStream = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else {
			throw std::runtime_error("Invalid parameter key \"" + setting.first + "\" for OStream appender");
		}
	}

	if(!errorStream) {
		throw std::runtime_error("Missing definition of parameter key \"error\" for OStream appender");
	}
	if(!warnStream) {
		throw std::runtime_error("Missing definition of parameter key \"warn\" for OStream appender");
	}
	if(!infoStream) {
		throw std::runtime_error("Missing definition of parameter key \"info\" for OStream appender");
	}
	if(!debugStream) {
		throw std::runtime_error("Missing definition of parameter key \"debug\" for OStream appender");
	}
	if(!traceStream) {
		throw std::runtime_error("Missing definition of parameter key \"trace\" for OStream appender");
	}

	return OStreamAppender::OStreams(*traceStream, *debugStream, *infoStream, *warnStream, *errorStream);
}

} /* anonymous namespce */

OStreamAppender::OStreams::OStreams(std::ostream& aTrace, std::ostream& aDebug, std::ostream& aInfo, std::ostream& aWarn, std::ostream& aError)
: trace(aTrace),
  debug(aDebug),
  info(aInfo),
  warn(aWarn),
  error(aError)
{ }

std::unique_ptr<esl::logging::Appender> OStreamAppender::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::logging::Appender>(new OStreamAppender(settings));
}

OStreamAppender::OStreamAppender(const std::vector<std::pair<std::string, std::string>>& settings)
: oStreams(getOStreams(settings))
{ }

void OStreamAppender::setLayout(const esl::logging::Layout* aLayout) {
	layout = aLayout;
}

const esl::logging::Layout* OStreamAppender::getLayout() const {
	return layout;
}

/* both methods are NOT thread-safe */
void OStreamAppender::setRecordLevel(esl::logging::Appender::RecordLevel aRecordLevel) {
	recordLevel = aRecordLevel;
}

esl::logging::Appender::RecordLevel OStreamAppender::getRecordLevel() const {
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

void OStreamAppender::write(const esl::logging::Location& aLocation, const char* ptr, std::size_t size) {
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

	if(lastLocation != aLocation) {
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

std::ostream& OStreamAppender::getOStream(esl::logging::Level level) {
	switch(level) {
	case esl::logging::Level::TRACE:
		return oStreams.trace;
	case esl::logging::Level::DEBUG:
		return oStreams.debug;
	case esl::logging::Level::INFO:
		return oStreams.info;
	case esl::logging::Level::WARN:
		return oStreams.warn;
	case esl::logging::Level::ERROR:
		return oStreams.error;
	default:
		break;
	}

	return oStreams.error;
}

} /* namespace logging */
} /* namespace common4esl */
