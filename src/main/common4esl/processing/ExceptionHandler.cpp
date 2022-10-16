#include <common4esl/ExceptionHandler.h>
#include <common4esl/Logger.h>
#include <common4esl/processing/ExceptionHandler.h>

#include <esl/object/Value.h>
#include <esl/utility/String.h>

#include <exception>
#include <sstream>
#include <stdexcept>

namespace common4esl {
namespace processing {

namespace {
Logger logger("common4esl::processing::ExceptionHandler");
} /* anonymous namespace */

std::unique_ptr<esl::processing::Procedure> ExceptionHandler::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::processing::Procedure>(new ExceptionHandler(settings));
}

ExceptionHandler::ExceptionHandler(const std::vector<std::pair<std::string, std::string>>& settings) {
	bool hasShowStacktrace = false;

	for(const auto& setting : settings) {
		if(setting.first == "handle-exception") {
			if(hasHandleException) {
		        throw std::runtime_error("multiple definition of attribute 'handle-exception'.");
			}
			hasHandleException = true;
			if(setting.second == "rethrow") {
				handleException = rethrow;
			}
			else if(setting.second == "stop") {
				handleException = stop;
			}
			else if(setting.second == "ignore") {
				handleException = ignore;
			}
			else {
		    	throw std::runtime_error("Invalid value \"" + setting.second + "\" for attribute 'show-exception'");
			}
		}
		else if(setting.first == "show-stacktrace") {
			if(hasShowStacktrace) {
		        throw std::runtime_error("multiple definition of attribute 'show-stacktrace'.");
			}
			std::string value = esl::utility::String::toLower(setting.second);
			hasShowStacktrace = true;
			if(value == "true") {
				showStacktrace = true;
			}
			else if(value == "false") {
				showStacktrace = false;
			}
			else {
		    	throw std::runtime_error("Invalid value \"" + setting.second + "\" for attribute 'show-stacktrace'");
			}
		}
		else if(setting.first == "show-output") {
			if(showOutput) {
		        throw std::runtime_error("multiple definition of attribute 'show-output'.");
			}

			if(setting.second == "stdout") {
				showOutput.reset(new ShowOutput(std::cout));
			}
			else if(setting.second == "stderr") {
				showOutput.reset(new ShowOutput(std::cerr));
			}
			else if(setting.second == "trace") {
				showOutput.reset(new ShowOutput(logger.trace));
			}
			else if(setting.second == "debug") {
				showOutput.reset(new ShowOutput(logger.debug));
			}
			else if(setting.second == "info") {
				showOutput.reset(new ShowOutput(logger.info));
			}
			else if(setting.second == "warn") {
				showOutput.reset(new ShowOutput(logger.warn));
			}
			else if(setting.second == "error") {
				showOutput.reset(new ShowOutput(logger.error));
			}
			else {
		    	throw std::runtime_error("Invalid value \"" + setting.second + "\" for attribute 'show-output'");
			}
		}
		else {
            throw std::runtime_error("unknown attribute '\"" + setting.first + "\"'.");
		}
    }

	if(hasShowStacktrace && showStacktrace && !showOutput) {
		logger.warn << "Definition of 'show-stacktrace' is useless if definition of 'show-output' is missing";
	}
}

void ExceptionHandler::procedureRun(esl::object::Context& context) {
	esl::object::Value<std::exception_ptr>* exceptionPointer = context.findObject<esl::object::Value<std::exception_ptr>>("exception");
	if(!exceptionPointer || !**exceptionPointer) {
		return;
	}

	if(showOutput) {
		common4esl::ExceptionHandler exceptionHandler(**exceptionPointer, showStacktrace, false);

		if(showOutput->ostream) {
			if(esl::logging::Logging::get()) {
				esl::logging::Logging::get()->flush(showOutput->ostream);
			}

			exceptionHandler.dump(*showOutput->ostream);
		}
		else if(showOutput->streamReal) {
			if(esl::logging::Logging::get()) {
				std::stringstream oStream;
				esl::logging::Logging::get()->flush(&oStream);
				(*showOutput->streamReal) << oStream.str();
			}

			exceptionHandler.dump(*showOutput->streamReal);
		}
	}

	switch(handleException) {
	case rethrow:
		std::rethrow_exception(**exceptionPointer);
		break;
	case ignore:
		**exceptionPointer = nullptr;
		break;
	case stop:
		break;
	}
}

} /* namespace processing */
} /* namespace common4esl */
