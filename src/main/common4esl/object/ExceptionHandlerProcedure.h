#ifndef COMMON4ESL_OBJECT_EXCEPTIONHANDLERPROCEDURE_H_
#define COMMON4ESL_OBJECT_EXCEPTIONHANDLERPROCEDURE_H_

#include <esl/object/ExceptionHandlerProcedure.h>
#include <esl/object/Procedure.h>

#include <esl/logging/Streams.h>
#include <esl/object/Context.h>

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
inline namespace v1_6 {
namespace object {

class ExceptionHandlerProcedure : public esl::object::Procedure {
public:
	ExceptionHandlerProcedure(const esl::object::ExceptionHandlerProcedure::Settings& settings);

	void procedureRun(esl::object::Context& context) override;

private:
	esl::object::ExceptionHandlerProcedure::Settings settings;
};

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_OBJECT_EXCEPTIONHANDLERPROCEDURE_H_ */
