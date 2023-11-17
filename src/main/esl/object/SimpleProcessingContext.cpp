#include <esl/object/SimpleProcessingContext.h>

#include <common4esl/object/ProcessingContext.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

namespace {
std::tuple<std::unique_ptr<Object>, ProcessingContext&, InitializeContext&> createSimpleProcessingContext(const SimpleProcessingContext::Settings& settings) {
	std::unique_ptr<common4esl::object::ProcessingContext> simpleProcessingContext(new common4esl::object::ProcessingContext(settings));

	ProcessingContext& processingContext = *simpleProcessingContext;
	InitializeContext& initContext = *simpleProcessingContext;
	std::unique_ptr<object::Object> object(simpleProcessingContext.release());

	return std::tuple<std::unique_ptr<Object>, ProcessingContext&, object::InitializeContext&>(std::move(object), processingContext, initContext);
}
}

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
: SimpleProcessingContext(createSimpleProcessingContext(settings))
{ }

SimpleProcessingContext::SimpleProcessingContext(std::tuple<std::unique_ptr<Object>, ProcessingContext&, InitializeContext&> aObject)
: object(std::move(aObject))
{ }

std::unique_ptr<ProcessingContext> SimpleProcessingContext::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<ProcessingContext>(new SimpleProcessingContext(Settings(settings)));
}

void SimpleProcessingContext::setParentObjectContext(Context* aParentObject) {
	std::get<1>(object).setParentObjectContext(aParentObject);
}

void SimpleProcessingContext::addAlias(const std::string& destinationId, const std::string& sourceId) {
	std::get<1>(object).addAlias(destinationId, sourceId);
}

void SimpleProcessingContext::addData(const std::string& configuration) {
	std::get<1>(object).addData(configuration);
}

void SimpleProcessingContext::addFile(const boost::filesystem::path& filename) {
	std::get<1>(object).addFile(filename);
}

int SimpleProcessingContext::getReturnCode() const {
	return std::get<1>(object).getReturnCode();
}

void SimpleProcessingContext::procedureRun(Context& context) {
	std::get<1>(object).procedureRun(context);
}

void SimpleProcessingContext::procedureCancel() {
	std::get<1>(object).procedureCancel();
}

void SimpleProcessingContext::onEvent(const esl::object::Object& aObject) {
	std::get<1>(object).onEvent(aObject);
}

io::Input SimpleProcessingContext::accept(com::common::server::RequestContext& requestContext) {
	return std::get<1>(object).accept(requestContext);
}

std::set<std::string> SimpleProcessingContext::getNotifiers() const {
	return std::get<1>(object).getNotifiers();
}

void SimpleProcessingContext::addObject(const std::string& id, std::unique_ptr<Object> aObject) {
	std::get<1>(object).addObject(id, std::move(aObject));
}

std::set<std::string> SimpleProcessingContext::getObjectIds() const {
	return std::get<1>(object).getObjectIds();
}

void SimpleProcessingContext::initializeContext(Context& context) {
	std::get<2>(object).initializeContext(context);
}

Object* SimpleProcessingContext::findRawObject(const std::string& id) {
	return std::get<1>(object).findObject<Object>(id);
}

const Object* SimpleProcessingContext::findRawObject(const std::string& id) const {
	return std::get<1>(object).findObject<Object>(id);
}


} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
