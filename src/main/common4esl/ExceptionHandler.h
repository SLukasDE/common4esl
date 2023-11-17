#ifndef COMMON4ESL_EXCEPTIONHANDLER_H_
#define COMMON4ESL_EXCEPTIONHANDLER_H_

#include <esl/monitoring/Streams.h>

#include <exception>
#include <ostream>
#include <string>
#include <vector>

namespace common4esl {
inline namespace v1_6 {

class ExceptionHandler {
public:
	ExceptionHandler(std::exception_ptr exceptionPointer, bool showStacktrace, bool showFilePosition);

	void dump(std::ostream& stream) const;
	void dump(esl::monitoring::Streams::Real& stream, esl::monitoring::Streams::Location location = esl::monitoring::Streams::Location{}) const;
	inline void dump(esl::monitoring::Streams::Empty& stream, esl::monitoring::Streams::Location location = esl::monitoring::Streams::Location{}) const { };

private:
	void initialize(std::exception_ptr exceptionPointer);
	template<typename E>
	void addEntry(const E& e, const std::string& aPlainException, const char* what, const std::string& details = "");

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

} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_EXCEPTIONHANDLER_H_ */
