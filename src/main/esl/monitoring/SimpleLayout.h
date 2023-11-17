#ifndef ESL_MONITORING_SIMPLELAYOUT_H_
#define ESL_MONITORING_SIMPLELAYOUT_H_

#include <esl/monitoring/Layout.h>
#include <esl/monitoring/Streams.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace monitoring {

class SimpleLayout : public Layout {
public:
	struct Settings {
		Settings() = default;
		Settings(const std::vector<std::pair<std::string, std::string>>& settings);

		bool showTimestamp = true;
		bool showLevel = true;
		bool showTypename = true;
		bool showAddress = true;
		bool showFile = false;
		bool showFunction = false;
		bool showLineNo = false;
		bool showThreadNo = true;
	};

	SimpleLayout(const Settings& settings);

	static std::unique_ptr<Layout> create(const std::vector<std::pair<std::string, std::string>>& settings);

	std::string toString(const Streams::Location& location) const override;

private:
	std::unique_ptr<Layout> layout;
};

} /* namespace monitoring */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_MONITORING_SIMPLELAYOUT_H_ */
