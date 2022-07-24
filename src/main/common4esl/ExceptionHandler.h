#ifndef COMMON4ESL_EXCEPTIONHANDLER_H_
#define COMMON4ESL_EXCEPTIONHANDLER_H_

#include <esl/logging/StreamReal.h>
#include <esl/logging/StreamEmpty.h>
#include <esl/logging/Location.h>

#include <exception>
#include <ostream>
#include <string>
#include <vector>

namespace common4esl {

class ExceptionHandler {
public:
	ExceptionHandler(std::exception_ptr exceptionPointer, bool showStacktrace, bool showFilePosition);

	void dump(std::ostream& stream) const;
	void dump(esl::logging::StreamReal& stream, esl::logging::Location location = esl::logging::Location{}) const;
	inline void dump(esl::logging::StreamEmpty& stream, esl::logging::Location location = esl::logging::Location{}) const { };

private:
	void initialize(std::exception_ptr exceptionPointer);
	void addEntry(const std::exception& e, const std::string& aPlainException, const char* what, const std::string& details = "");

	bool showStacktrace;
	bool showFilePosition;

	struct Entry {
		std::string exceptionType;
		std::string what;
		std::string details;

		std::string stacktrace;
		std::string fileMessage;
	};
	std::vector<Entry> entries;
};

} /* namespace common4esl */

#endif /* COMMON4ESL_EXCEPTIONHANDLER_H_ */
