#ifndef COMMON4ESL_MONITORING_DEFAULTLAYOUT_H_
#define COMMON4ESL_MONITORING_DEFAULTLAYOUT_H_

#include <esl/monitoring/Layout.h>
#include <esl/monitoring/SimpleLayout.h>

#include <esl/monitoring/Streams.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
inline namespace v1_6 {
namespace monitoring {

class DefaultLayout : public esl::monitoring::Layout {
public:
	DefaultLayout(const esl::monitoring::SimpleLayout::Settings& settings);

	std::string toString(const esl::monitoring::Streams::Location& location) const override;

private:
	esl::monitoring::SimpleLayout::Settings settings;
};

} /* namespace monitoring */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_MONITORING_DEFAULTLAYOUT_H_ */
