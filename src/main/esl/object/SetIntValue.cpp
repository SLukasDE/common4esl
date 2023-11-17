#include <esl/object/SetIntValue.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

std::unique_ptr<Object> SetIntValue::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	std::set<int> value;

	for(const auto& setting : settings) {
		if(setting.first == "value") {
			value.insert(std::stoi(setting.second));
		}
		else {
			throw std::runtime_error("Unknown parameter key=\"" + setting.first + "\" with value=\"" + setting.second + "\"");
		}
	}

	return std::unique_ptr<Object>(new SetIntValue(std::move(value)));
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
