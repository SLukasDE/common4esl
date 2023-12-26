#ifndef COMMON4ESL_OBJECT_PROCESSINGCONTEXTENTRY_H_
#define COMMON4ESL_OBJECT_PROCESSINGCONTEXTENTRY_H_

#include <esl/object/Context.h>
#include <esl/object/InitializeContext.h>
#include <esl/object/Object.h>
#include <esl/object/Procedure.h>

#include <memory>

namespace common4esl {
inline namespace v1_6 {
namespace object {

class ProcessingContextEntry {
public:
	ProcessingContextEntry(std::unique_ptr<esl::object::Object> object);
	ProcessingContextEntry(esl::object::Object& refObject);

	void initializeContext(esl::object::Context& context);
	void procedureRun(esl::object::Context& context);
	void procedureCancel();

private:
	std::unique_ptr<esl::object::Object> object;
	esl::object::Object& refObject;

	esl::object::InitializeContext* initializeContextPtr = nullptr;
	esl::object::Procedure* procedure = nullptr;
};

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_OBJECT_PROCESSINGCONTEXTENTRY_H_ */
