#ifndef COMMON4ESL_HTTPSTATUSCODE_H_
#define COMMON4ESL_HTTPSTATUSCODE_H_

#include <string>

namespace common4esl {
inline namespace v1_6 {

struct HttpStatusCode final {
	HttpStatusCode() = delete;

	static std::string getMessage(short int statusCode);
};

} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_HTTPSTATUSCODE_H_ */
