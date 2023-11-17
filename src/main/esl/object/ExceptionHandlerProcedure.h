#ifndef ESL_OBJECT_EXCEPTIONHANDLERPROCEDURE_H_
#define ESL_OBJECT_EXCEPTIONHANDLERPROCEDURE_H_

#include <esl/logging/Streams.h>
#include <esl/object/Context.h>
#include <esl/object/Procedure.h>

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace object {

class ExceptionHandlerProcedure : public Procedure {
public:
	struct Settings {
		Settings(const std::vector<std::pair<std::string, std::string>>& settings);
		enum HandleException {
			rethrow,
			stop,
			ignore
		};

		HandleException handleException = stop;
		bool showStacktrace = true;
		std::ostream* showOutputOStream = nullptr;
		esl::logging::Streams::Real* showOutputStreamReal = nullptr;
	};

	ExceptionHandlerProcedure(const Settings& settings);

	static std::unique_ptr<Procedure> create(const std::vector<std::pair<std::string, std::string>>& settings);

	void procedureRun(Context& context) override;

	/* this method is non-blocking. */
	void procedureCancel() override;

private:
	std::unique_ptr<Procedure> procedure;
};

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_OBJECT_EXCEPTIONHANDLERPROCEDURE_H_ */
