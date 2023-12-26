#include <common4esl/object/ProcessingContext.h>
#include <common4esl/object/ProcessingContextEntry.h>

namespace common4esl {
inline namespace v1_6 {
namespace object {

ProcessingContextEntry::ProcessingContextEntry(std::unique_ptr<esl::object::Object> aObject)
: object(std::move(aObject)),
  refObject(*object),
  initializeContextPtr(dynamic_cast<esl::object::InitializeContext*>(object.get())),
  procedure(dynamic_cast<esl::object::Procedure*>(&refObject))
{ }

ProcessingContextEntry::ProcessingContextEntry(esl::object::Object& refObject)
: refObject(refObject),
  initializeContextPtr(nullptr),
  procedure(dynamic_cast<esl::object::Procedure*>(&refObject))
{ }


void ProcessingContextEntry::initializeContext(esl::object::Context& context) {
	if(initializeContextPtr) {
		initializeContextPtr->initializeContext(context);
		initializeContextPtr = nullptr;
	}
}

void ProcessingContextEntry::procedureRun(esl::object::Context& context) {
	if(procedure) {
		procedure->procedureRun(context);
	}
}

void ProcessingContextEntry::procedureCancel() {
	if(procedure) {
		procedure->procedureCancel();
	}
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace common4esl */
