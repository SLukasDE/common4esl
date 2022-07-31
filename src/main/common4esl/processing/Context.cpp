#include <common4esl/config/context/Context.h>
#include <common4esl/ExceptionHandler.h>
#include <common4esl/Logger.h>
#include <common4esl/processing/Context.h>

#include <esl/com/http/server/exception/StatusCode.h>
#include <esl/database/exception/SqlError.h>
#include <esl/logging/Logging.h>
#include <esl/utility/String.h>

#include <stdexcept>
#include <sstream>

namespace common4esl {
namespace processing {

namespace {
Logger logger("common4esl::processing::Context");
} /* anonymous namespace */

std::unique_ptr<esl::processing::Context> Context::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::processing::Context>(new Context(settings));
}

Context::Context(const std::vector<std::pair<std::string, std::string>>& settings) {
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
			else if(setting.second == "stop-and-show") {
				handleException = stopAndShow;
			}
			else if(setting.second == "ignore") {
				handleException = ignore;
			}
			else if(setting.second == "ignore-and-show") {
				handleException = ignoreAndShow;
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

	if(!showOutput && handleException == stopAndShow) {
        throw std::runtime_error("Definition of 'handle-exception' = 'stop-and-show' without definition of 'show-output'.");
	}
	if(!showOutput && handleException == ignoreAndShow) {
        throw std::runtime_error("Definition of 'handle-exception' = 'ignore-and-show' without definition of 'show-output'.");
	}
	if(showOutput && handleException != stopAndShow && handleException != ignoreAndShow) {
		logger.warn << "Definition of 'show-output' is useless if definition of 'handle-exception' is neither 'stop-and-show' nor 'ignore-and-show'";
	}
	if(hasShowStacktrace && showStacktrace && handleException != stopAndShow && handleException != ignoreAndShow) {
		logger.warn << "Definition of 'show-stacktrace' is useless if definition of 'handle-exception' is neither 'stop-and-show' nor 'ignore-and-show'";
	}
}

void Context::setParent(Context* parentContext) {
	parent = parentContext;

	if(!hasHandleException) {
		handleException = parent ? rethrow : showOutput ? stopAndShow : stop;
	}
}

esl::processing::Context& Context::addData(const std::string& configuration) {
	config::context::Context config(configuration);

	config.loadLibraries();
	config.install(*this);

	return *this;
}

esl::processing::Context& Context::addFile(const boost::filesystem::path& filename) {
	config::context::Context config(filename);

	config.loadLibraries();
	config.install(*this);

	return *this;
}

esl::processing::Context& Context::addReference(const std::string& destinationId, const std::string& sourceId) {
	esl::object::Object* object = findRawObject(sourceId);

	if(object == nullptr) {
        throw std::runtime_error("Cannot add reference with id '" + destinationId + "' because source object with id '" + sourceId + "' does not exists.");
	}

	if(destinationId.empty()) {
		entries.push_back(std::unique_ptr<ContextEntry>(new ContextEntry(*object)));
	}
	else {
		if(objects.count(destinationId) == 0) {
	        throw std::runtime_error("Cannot add reference with id '" + destinationId + "' because there exists already an object with same id.");
		}
		objects.insert(std::make_pair(destinationId, IdElement(*object)));
	}

	return *this;
}

int Context::getReturnCode() const {
	return returnCode;
}

void Context::onEvent(const esl::object::Object& object) {
	initializeContext(*this);

	for(auto& entry : entries) {
		entry->onEvent(object);
	}
}

std::set<std::string> Context::getObjectIds() const {
	std::set<std::string> rv;

	for(const auto& object : objects) {
		rv.insert(object.first);
	}

	return rv;
}

void Context::procedureRun(esl::object::Context& context) {
	initializeContext(*this);

	for(auto& entry : entries) {
		if(handleException == rethrow) {
			entry->procedureRun(context);
		}
		else {
			try {
				entry->procedureRun(context);
			}
			catch(...) {
				if(showOutput && (handleException == stopAndShow || handleException == ignoreAndShow)) {
					if(showOutput->ostream) {
						if(esl::logging::Logging::get()) {
							esl::logging::Logging::get()->flush(showOutput->ostream);
						}
						ExceptionHandler(std::current_exception(), showStacktrace, showFilePosition).dump(*showOutput->ostream);
						//printException(*showOutput->ostream, std::current_exception());
					}
					else if(showOutput->streamReal) {
						std::stringstream oStream;
						if(esl::logging::Logging::get()) {
							esl::logging::Logging::get()->flush(&oStream);
						}
						(*showOutput->streamReal) << oStream.str();
						ExceptionHandler(std::current_exception(), showStacktrace, showFilePosition).dump(*showOutput->streamReal);
						//printException(*showOutput->streamReal, std::current_exception(), esl::logging::Location{});
					}
				}

				if(handleException == stop || handleException == stopAndShow) {
					esl::object::Object* objectPtr = context.findObject<esl::object::Object>("exception");
					esl::object::Value<std::exception_ptr>* exceptionObjectPtr = dynamic_cast<esl::object::Value<std::exception_ptr>*>(objectPtr);

					if(exceptionObjectPtr) {
						*exceptionObjectPtr = std::current_exception();
					}
					else if(objectPtr == nullptr) {
						context.addObject("exception", std::unique_ptr<esl::object::Value<std::exception_ptr>>(new esl::object::Value<std::exception_ptr>(std::current_exception())));
					}

					break;
				}
			}
		}
	}

	ReturnCodeObject* returnCodeObject = context.findObject<ReturnCodeObject>("return-code");
	if(returnCodeObject) {
		returnCode = **returnCodeObject;
	}
}

void Context::initializeContext(esl::object::Context&) {
	for(auto& entry : entries) {
		entry->initializeContext(*this);
	}
	for(auto& object : objects) {
		if(object.second.initializeContext) {
			object.second.initializeContext->initializeContext(*this);
			object.second.initializeContext = nullptr;
		}
	}
}

esl::object::Object* Context::findRawObject(const std::string& id) {
	auto iter = objects.find(id);
	esl::object::Object* object = iter == std::end(objects) ? nullptr : &iter->second.refObject;
	if(object) {
		return object;
	}
	if(parent) {
		parent->findObject<esl::object::Object>(id);
	}
	return nullptr;
}

const esl::object::Object* Context::findRawObject(const std::string& id) const {
	auto iter = objects.find(id);
	esl::object::Object* object = iter == std::end(objects) ? nullptr : &iter->second.refObject;
	if(object) {
		return object;
	}
	if(parent) {
		parent->findObject<esl::object::Object>(id);
	}
	return nullptr;
}

void Context::addRawObject(const std::string& id, std::unique_ptr<esl::object::Object> object) {
	Context* context = dynamic_cast<Context*>(object.get());

	if(id.empty()) {
		entries.push_back(std::unique_ptr<ContextEntry>(new ContextEntry(std::move(object))));
	}
	else if(objects.insert(std::make_pair(id, IdElement(std::move(object)))).second == false) {
        throw std::runtime_error("Cannot add object with id '" + id + "' because there exists already an object with same id.");
	}

	if(context) {
		context->setParent(this);
	}
}

} /* namespace processing */
} /* namespace common4esl */
