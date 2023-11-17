#include <common4esl/object/ProcessingContext.h>
#include <common4esl/object/ProcessingContextEntry.h>

namespace common4esl {
inline namespace v1_6 {
namespace object {

ProcessingContextEntry::ProcessingContextEntry(std::unique_ptr<esl::object::Object> aObject)
: object(std::move(aObject)),
  refObject(*object),
  initializeContextPtr(dynamic_cast<esl::object::InitializeContext*>(object.get())),
  //context(dynamic_cast<IContext*>(object.get())),
  event(dynamic_cast<esl::object::Event*>(object.get())),
  procedure(dynamic_cast<esl::object::Procedure*>(&refObject))
{ }

ProcessingContextEntry::ProcessingContextEntry(esl::object::Object& refObject)
: refObject(refObject),
  initializeContextPtr(nullptr),
  event(dynamic_cast<esl::object::Event*>(&refObject)),
  procedure(dynamic_cast<esl::object::Procedure*>(&refObject))
{ }


void ProcessingContextEntry::initializeContext(esl::object::Context& context) {
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

void ProcessingContextEntry::onEvent(const esl::object::Object& object) {
	if(event) {
		event->onEvent(object);
	}
}

void ProcessingContextEntry::procedureRun(esl::object::Context& context) {
	if(procedure) {
		procedure->procedureRun(context);
	}

	/* wurde schon oben aufgerufen, weil jeder ESL-Boot-Context das Interface "procedure" realisiert
	if(context) {
		context->procedureRun(objectContext);
	}
	*/
}

void ProcessingContextEntry::procedureCancel() {
	if(procedure) {
		procedure->procedureCancel();
	}
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace common4esl */
