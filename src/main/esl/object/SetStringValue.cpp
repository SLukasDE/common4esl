#include <esl/object/SetStringValue.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

std::unique_ptr<Object> SetStringValue::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	std::set<std::string> value;

	for(const auto& setting : settings) {
		if(setting.first == "value") {
			value.insert(setting.second);
		}
		else {
			throw std::runtime_error("Unknown parameter key=\"" + setting.first + "\" with value=\"" + setting.second + "\"");
		}
	}

	return std::unique_ptr<Object>(new SetStringValue(std::move(value)));
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
