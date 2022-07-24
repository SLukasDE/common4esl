#include <common4esl/processing/Context.h>
#include <common4esl/processing/ContextEntry.h>

namespace common4esl {
namespace processing {

ContextEntry::ContextEntry(std::unique_ptr<esl::object::Object> aObject)
: object(std::move(aObject)),
  refObject(*object),
  initializeContextPtr(dynamic_cast<esl::object::InitializeContext*>(object.get())),
  //context(dynamic_cast<IContext*>(object.get())),
  event(dynamic_cast<esl::object::Event*>(object.get())),
  procedure(dynamic_cast<esl::processing::Procedure*>(&refObject))
{ }

ContextEntry::ContextEntry(esl::object::Object& refObject)
: refObject(refObject),
  initializeContextPtr(nullptr),
  event(dynamic_cast<esl::object::Event*>(&refObject)),
  procedure(dynamic_cast<esl::processing::Procedure*>(&refObject))
{ }


void ContextEntry::initializeContext(esl::object::Context& context) {
	if(initializeContextPtr) {
		initializeContextPtr->initializeContext(context);
		initializeContextPtr = nullptr;
	}

	/* wurde schon oben aufgerufen, weil der JBoot-Context das Interface "InitializeContext" realisiert
	if(context) {
		context->initializeContext(objectContext);
	}
	*/
}

void ContextEntry::onEvent(const esl::object::Object& object) {
	if(event) {
		event->onEvent(object);
	}
}

void ContextEntry::procedureRun(esl::object::Context& context) {
	if(procedure) {
		procedure->procedureRun(context);
	}

	/* wurde schon oben aufgerufen, weil jeder ESL-Boot-Context das Interface "procedure" realisiert
	if(context) {
		context->procedureRun(objectContext);
	}
	*/
}

void ContextEntry::procedureCancel() {
	if(procedure) {
		procedure->procedureCancel();
	}
}

} /* namespace processing */
} /* namespace common4esl */
