#ifndef COMMON4ESL_LOGGING_DEFAULTLAYOUT_H_
#define COMMON4ESL_LOGGING_DEFAULTLAYOUT_H_

#include <esl/logging/Layout.h>
#include <esl/logging/SimpleLayout.h>

#include <esl/logging/Streams.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
inline namespace v1_6 {
namespace logging {

class DefaultLayout : public esl::logging::Layout {
public:
	DefaultLayout(const esl::logging::SimpleLayout::Settings& settings);

	std::string toString(const esl::logging::Streams::Location& location) const override;

private:
	esl::logging::SimpleLayout::Settings settings;
};

} /* namespace logging */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_LOGGING_DEFAULTLAYOUT_H_ */
