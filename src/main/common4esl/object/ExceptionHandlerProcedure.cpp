#include <common4esl/ExceptionHandler.h>
#include <common4esl/object/ExceptionHandlerProcedure.h>

#include <esl/object/Value.h>
#include <esl/utility/String.h>

#include <esl/monitoring/Logging.h>
#include <esl/plugin/Registry.h>

#include <exception>
#include <sstream>
#include <stdexcept>

namespace common4esl {
inline namespace v1_6 {
namespace object {

ExceptionHandlerProcedure::ExceptionHandlerProcedure(const esl::object::ExceptionHandlerProcedure::Settings& aSettings)
: settings(aSettings)
{ }

void ExceptionHandlerProcedure::procedureRun(esl::object::Context& context) {
	esl::object::Value<std::exception_ptr>* exceptionPointer = context.findObject<esl::object::Value<std::exception_ptr>>("exception");
	if(!exceptionPointer || !**exceptionPointer) {
		return;
	}

	if(settings.showOutputOStream || settings.showOutputStreamReal) {
		common4esl::ExceptionHandler exceptionHandler(**exceptionPointer, settings.showStacktrace, false);

		if(settings.showOutputOStream) {
			if(esl::plugin::Registry::get().findObject<esl::monitoring::Logging>()) {
				esl::plugin::Registry::get().findObject<esl::monitoring::Logging>()->flush(settings.showOutputOStream);
			}

			exceptionHandler.dump(*settings.showOutputOStream);
		}
		else if(settings.showOutputStreamReal) {
			if(esl::plugin::Registry::get().findObject<esl::monitoring::Logging>()) {
				std::stringstream oStream;
				esl::plugin::Registry::get().findObject<esl::monitoring::Logging>()->flush(&oStream);
				(*settings.showOutputStreamReal) << oStream.str();
			}

			exceptionHandler.dump(*settings.showOutputStreamReal);
		}
	}

	switch(settings.handleException) {
	case esl::object::ExceptionHandlerProcedure::Settings::rethrow:
		std::rethrow_exception(**exceptionPointer);
		break;
	case esl::object::ExceptionHandlerProcedure::Settings::ignore:
		**exceptionPointer = nullptr;
		break;
	case esl::object::ExceptionHandlerProcedure::Settings::stop:
		break;
	}
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace common4esl */
