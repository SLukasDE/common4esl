#ifndef COMMON4ESL_PROCESSING_EXCEPTIONHANDLER_H_
#define COMMON4ESL_PROCESSING_EXCEPTIONHANDLER_H_

#include <esl/logging/StreamEmpty.h>
#include <esl/logging/StreamReal.h>
#include <esl/object/Context.h>
#include <esl/processing/Procedure.h>

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
namespace processing {

class ExceptionHandler : public esl::processing::Procedure {
public:
	static std::unique_ptr<esl::processing::Procedure> create(const std::vector<std::pair<std::string, std::string>>& settings);

	ExceptionHandler(const std::vector<std::pair<std::string, std::string>>& settings);

	void procedureRun(esl::object::Context& context) override;

private:
	enum HandleException {
		rethrow,
		stop,
		ignore
	};
	bool hasHandleException = false;
	HandleException handleException = stop;

	bool showStacktrace = true;

	struct ShowOutput {
		ShowOutput(std::ostream& aOStream)
		: ostream(&aOStream)
		{}

		ShowOutput(esl::logging::StreamReal& aStreamReal)
		: streamReal(&aStreamReal)
		{}

		ShowOutput(esl::logging::StreamEmpty& aStreamEmpty)
		: streamEmpty(&aStreamEmpty)
		{}

		std::ostream* ostream = nullptr;
		esl::logging::StreamReal* streamReal = nullptr;
		esl::logging::StreamEmpty* streamEmpty = nullptr;
	};
	std::unique_ptr<ShowOutput> showOutput;
};

} /* namespace processing */
} /* namespace common4esl */

#endif /* COMMON4ESL_PROCESSING_EXCEPTIONHANDLER_H_ */
