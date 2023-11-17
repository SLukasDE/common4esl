#include <esl/object/VectorStringValue.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

std::unique_ptr<Object> VectorStringValue::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	std::vector<std::string> value;

	for(const auto& setting : settings) {
		if(setting.first == "value") {
			value.push_back(setting.second);
		}
		else {
			throw std::runtime_error("Unknown parameter key=\"" + setting.first + "\" with value=\"" + setting.second + "\"");
		}
	}

	return std::unique_ptr<Object>(new VectorStringValue(std::move(value)));
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
