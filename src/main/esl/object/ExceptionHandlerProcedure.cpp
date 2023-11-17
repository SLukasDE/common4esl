#include <esl/Logger.h>
#include <esl/object/ExceptionHandlerProcedure.h>
#include <esl/object/Procedure.h>
#include <esl/utility/String.h>

#include <common4esl/object/ExceptionHandlerProcedure.h>

#include <iostream>

namespace esl {
inline namespace v1_6 {
namespace object {

namespace {
esl::Logger logger("esl::object::ExceptionHandlerProcedure");

void setShowOutput(ExceptionHandlerProcedure::Settings& settings, esl::logging::Streams::Real& aStreamReal) {
	settings.showOutputStreamReal = &aStreamReal;
}

void setShowOutput(ExceptionHandlerProcedure::Settings& settings, esl::logging::Streams::Empty& aStreamEmpty) {
}
}

ExceptionHandlerProcedure::Settings::Settings(const std::vector<std::pair<std::string, std::string>>& settings) {
	bool hasShowOutput = false;
	bool hasShowStacktrace = false;
	bool hasHandleException = false;

	for(const auto& setting : settings) {
		if(setting.first == "handle-exception") {
			if(hasHandleException) {
		        throw std::runtime_error("multiple definition of attribute 'handle-exception'.");
			}
			hasHandleException = true;
			if(setting.second == "rethrow") {
				handleException = ExceptionHandlerProcedure::Settings::rethrow;
			}
			else if(setting.second == "stop") {
				handleException = ExceptionHandlerProcedure::Settings::stop;
			}
			else if(setting.second == "ignore") {
				handleException = ExceptionHandlerProcedure::Settings::ignore;
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
			if(hasShowOutput) {
		        throw std::runtime_error("multiple definition of attribute 'show-output'.");
			}

			hasShowOutput = true;
			if(setting.second == "stdout") {
				showOutputOStream = &std::cout;
			}
			else if(setting.second == "stderr") {
				showOutputOStream = &std::cerr;
			}
			else if(setting.second == "trace") {
				setShowOutput(*this, logger.trace);
			}
			else if(setting.second == "debug") {
				setShowOutput(*this, logger.debug);
			}
			else if(setting.second == "info") {
				setShowOutput(*this, logger.info);
			}
			else if(setting.second == "warn") {
				setShowOutput(*this, logger.warn);
			}
			else if(setting.second == "error") {
				setShowOutput(*this, logger.error);
			}
			else {
		    	throw std::runtime_error("Invalid value \"" + setting.second + "\" for attribute 'show-output'");
			}
		}
		else {
            throw std::runtime_error("unknown attribute '\"" + setting.first + "\"'.");
		}
    }

	if(hasShowStacktrace && showStacktrace && !hasShowOutput) {
		logger.warn << "Definition of 'show-stacktrace' is useless if definition of 'show-output' is missing";
	}
}

ExceptionHandlerProcedure::ExceptionHandlerProcedure(const Settings& settings)
: procedure(new common4esl::object::ExceptionHandlerProcedure(settings))
{ }

std::unique_ptr<Procedure> ExceptionHandlerProcedure::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<Procedure>(new ExceptionHandlerProcedure(Settings(settings)));
}

void ExceptionHandlerProcedure::procedureRun(Context& context) {
	procedure->procedureRun(context);
}

/* this method is non-blocking. */
void ExceptionHandlerProcedure::procedureCancel() {
	procedure->procedureCancel();
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
