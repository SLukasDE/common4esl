#ifndef COMMON4ESL_PROCESSING_CONTEXTENTRY_H_
#define COMMON4ESL_PROCESSING_CONTEXTENTRY_H_

#include <esl/object/Event.h>
#include <esl/object/Object.h>
#include <esl/object/InitializeContext.h>
#include <esl/object/Context.h>
#include <esl/processing/Procedure.h>

#include <memory>

namespace common4esl {
namespace processing {

class ContextEntry {
public:
	ContextEntry(std::unique_ptr<esl::object::Object> object);
	ContextEntry(esl::object::Object& refObject);

	void initializeContext(esl::object::Context& context);
	void onEvent(const esl::object::Object& object);
	void procedureRun(esl::object::Context& context);
	void procedureCancel();

private:
	std::unique_ptr<esl::object::Object> object;
	esl::object::Object& refObject;

	esl::object::InitializeContext* initializeContextPtr = nullptr;
	//IContext* context = nullptr;
	esl::object::Event* event = nullptr;
	esl::processing::Procedure* procedure = nullptr;
};

} /* namespace processing */
} /* namespace common4esl */

#endif /* COMMON4ESL_PROCESSING_CONTEXTENTRY_H_ */
