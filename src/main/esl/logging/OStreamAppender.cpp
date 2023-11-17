#include <esl/logging/OStreamAppender.h>

#include <common4esl/logging/OStreamAppender.h>

#include <iostream>
#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace logging {

namespace {
OStreamAppender::Settings createOStreamSettings(const std::vector<std::pair<std::string, std::string>>& settings) {
	std::ostream* trace = nullptr;
	std::ostream* debug = nullptr;
	std::ostream* info = nullptr;
	std::ostream* warn = nullptr;
	std::ostream* error = nullptr;

	for(auto const& setting : settings) {
		if(setting.first == "error") {
			if(error) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				error = &std::cout;
			}
			else if(setting.second == "err") {
				error = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else if(setting.first == "warn") {
			if(warn) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				warn = &std::cout;
			}
			else if(setting.second == "err") {
				warn = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else if(setting.first == "info") {
			if(info) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				info = &std::cout;
			}
			else if(setting.second == "err") {
				info = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else if(setting.first == "debug") {
			if(debug) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				debug = &std::cout;
			}
			else if(setting.second == "err") {
				debug = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else if(setting.first == "trace") {
			if(trace) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for OStream appender");
			}

			if(setting.second == "out") {
				trace = &std::cout;
			}
			else if(setting.second == "err") {
				trace = &std::cerr;
			}
			else {
				throw std::runtime_error("Invalid value \"" + setting.second + "\" of parameter key \"" + setting.first + "\" for OStream appender");
			}
		}
		else {
			throw std::runtime_error("Invalid parameter key \"" + setting.first + "\" for OStream appender");
		}
	}

	if(!error) {
		throw std::runtime_error("Missing definition of parameter key \"error\" for OStream appender");
	}
	if(!warn) {
		throw std::runtime_error("Missing definition of parameter key \"warn\" for OStream appender");
	}
	if(!info) {
		throw std::runtime_error("Missing definition of parameter key \"info\" for OStream appender");
	}
	if(!debug) {
		throw std::runtime_error("Missing definition of parameter key \"debug\" for OStream appender");
	}
	if(!trace) {
		throw std::runtime_error("Missing definition of parameter key \"trace\" for OStream appender");
	}

	return OStreamAppender::Settings(*trace, *debug, *info, *warn, *error);
}
}

OStreamAppender::Settings::Settings(const std::vector<std::pair<std::string, std::string>>& settings)
: OStreamAppender::Settings(createOStreamSettings(settings))
{ }

OStreamAppender::Settings::Settings(std::ostream& aTrace, std::ostream& aDebug, std::ostream& aInfo, std::ostream& aWarn, std::ostream& aError)
: trace(aTrace),
  debug(aDebug),
  info(aInfo),
  warn(aWarn),
  error(aError)
{ }

OStreamAppender::OStreamAppender(const Settings& settings)
: appender(new common4esl::logging::OStreamAppender(settings))
{ }

std::unique_ptr<Appender> OStreamAppender::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<Appender>(new OStreamAppender(Settings(settings)));
}

void OStreamAppender::setLayout(const Layout* aLayout) {
	appender->setLayout(aLayout);
}

const Layout* OStreamAppender::getLayout() const {
	return appender->getLayout();
}

void OStreamAppender::setRecordLevel(RecordLevel aRecordLevel) {
	appender->setRecordLevel(aRecordLevel);
}

Appender::RecordLevel OStreamAppender::getRecordLevel() const {
	return appender->getRecordLevel();
}

void OStreamAppender::flush(std::ostream* oStream) {
	appender->flush(oStream);
}

void OStreamAppender::write(const Streams::Location& location, const char* ptr, std::size_t size) {
	appender->write(location, ptr, size);
}

} /* namespace logging */
} /* inline namespace v1_6 */
} /* namespace esl */
