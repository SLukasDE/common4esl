#include <esl/object/SimpleProcessingContext.h>

#include <common4esl/object/ProcessingContext.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

SimpleProcessingContext::Settings::Settings(const std::vector<std::pair<std::string, std::string>>& settings) {
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
			throw std::runtime_error("Unknown key \"" + setting.first + "\"");
		}
    }
}

SimpleProcessingContext::SimpleProcessingContext(const Settings& settings)
: object(createNative(settings))
{ }

std::unique_ptr<ProcessingContext> SimpleProcessingContext::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<ProcessingContext>(new SimpleProcessingContext(Settings(settings)));
}

std::unique_ptr<SimpleProcessingContextInterface> SimpleProcessingContext::createNative(const Settings& settings) {
	return std::unique_ptr<SimpleProcessingContextInterface>(new common4esl::object::ProcessingContext(settings));
}

void SimpleProcessingContext::setParentObjectContext(Context* aParentObject) {
	object->setParentObjectContext(aParentObject);
}

void SimpleProcessingContext::addAlias(const std::string& destinationId, const std::string& sourceId) {
	object->addAlias(destinationId, sourceId);
}

std::unique_ptr<Object> SimpleProcessingContext::runCommand(const std::string& command, Object* argument) {
	return object->runCommand(command, argument);
}

int SimpleProcessingContext::getReturnCode() const {
	return object->getReturnCode();
}

void SimpleProcessingContext::procedureRun(Context& context) {
	object->procedureRun(context);
}

void SimpleProcessingContext::procedureCancel() {
	object->procedureCancel();
}

io::Input SimpleProcessingContext::accept(com::common::server::RequestContext& requestContext) {
	return object->accept(requestContext);
}

std::set<std::string> SimpleProcessingContext::getNotifiers() const {
	return object->getNotifiers();
}

void SimpleProcessingContext::addObject(const std::string& id, std::unique_ptr<Object> aObject) {
	object->addObject(id, std::move(aObject));
}

std::set<std::string> SimpleProcessingContext::getObjectIds() const {
	return object->getObjectIds();
}

void SimpleProcessingContext::initializeContext(Context& context) {
	object->initializeContext(context);
}

Object* SimpleProcessingContext::findRawObject(const std::string& id) {
	return object->findObject<Object>(id);
}

const Object* SimpleProcessingContext::findRawObject(const std::string& id) const {
	return object->findObject<Object>(id);
}


} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
