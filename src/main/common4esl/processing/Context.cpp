#include <common4esl/config/context/Context.h>
#include <common4esl/Logger.h>
//#include <common4esl/processing/ExceptionHandler.h>
#include <common4esl/processing/Context.h>

#include <esl/com/http/server/exception/StatusCode.h>
#include <esl/database/exception/SqlError.h>
#include <esl/logging/Logging.h>
#include <esl/utility/String.h>

#include <exception>
#include <sstream>
#include <stdexcept>

namespace common4esl {
namespace processing {

namespace {
Logger logger("common4esl::processing::Context");
} /* anonymous namespace */

Context::IdElement::IdElement(std::unique_ptr<esl::object::Object> aObject)
: object(std::move(aObject)),
  refObject(*object),
  initializeContext(dynamic_cast<esl::object::InitializeContext*>(&refObject))
{ }

Context::IdElement::IdElement(esl::object::Object& refObject)
: refObject(refObject),
  initializeContext(nullptr)
{ }

std::unique_ptr<esl::processing::Context> Context::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::processing::Context>(new Context(settings));
}

Context::Context(const std::vector<std::pair<std::string, std::string>>& settings) {
	for(const auto& setting : settings) {
		if(setting.first == "exception-handler") {
			if(!exceptionHandlerId.empty()) {
		        throw std::runtime_error("multiple definition of attribute 'exception-handler'.");
			}
			exceptionHandlerId = setting.second;
			if(exceptionHandlerId.empty()) {
		    	throw std::runtime_error("Invalid value \"\" for attribute 'exception-handler'");
			}

		}
		else {
            throw std::runtime_error("unknown attribute '\"" + setting.first + "\"'.");
		}
    }
}

void Context::setParent(Context* parentContext) {
	parent = parentContext;
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
	if(!isInitialized) {
		initializeContext(*this);
	}

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
	if(!isInitialized) {
		initializeContext(*this);
	}

	for(auto& entry : entries) {
		if(!exceptionHandler) {
			entry->procedureRun(context);
			continue;
		}

		try {
			entry->procedureRun(context);
		}
		catch(...) {
			esl::object::Object* object = context.findObject<esl::object::Object>("exception");
			esl::object::Value<std::exception_ptr>* exceptionObjectPtr = nullptr;

			if(object) {
				exceptionObjectPtr = dynamic_cast<esl::object::Value<std::exception_ptr>*>(object);
				if(!exceptionObjectPtr) {
					logger.warn << "Object with id \"exception\" is not Value<std::exception_ptr>\n";
				}
			}
			else {
				context.addObject("exception", std::unique_ptr<esl::object::Value<std::exception_ptr>>(new esl::object::Value<std::exception_ptr>(std::current_exception())));
				exceptionObjectPtr = context.findObject<esl::object::Value<std::exception_ptr>>("exception");
			}

			exceptionHandler->procedureRun(context);

			if(exceptionObjectPtr && **exceptionObjectPtr) {
				break;
			}
		}
	}

	/* check if there is a return-code available */
	esl::object::Object* returnCodeObject = context.findObject<esl::object::Object>("return-code");
	if(returnCodeObject) {
		esl::object::Value<int>* returnCodeInt = dynamic_cast<esl::object::Value<int>*>(returnCodeObject);
		if(returnCodeInt) {
			returnCode = **returnCodeInt;
			logger.trace << "Found object \"return-code\" has integer value " << returnCode << "\n";
		}
		else {
			logger.warn << "Object with id \"return-code\" is not Value<int>\n";
		}
	}
}

void Context::initializeContext(esl::object::Context& context) {
	isInitialized = true;
	if(!exceptionHandlerId.empty()) {
		exceptionHandler = &getObject<esl::processing::Procedure>(exceptionHandlerId);
	}

	for(auto& entry : entries) {
		entry->initializeContext(context);
	}
	for(auto& object : objects) {
		if(object.second.initializeContext) {
			object.second.initializeContext->initializeContext(context);
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
