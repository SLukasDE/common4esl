#include <esl/object/IntValue.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

std::unique_ptr<Object> IntValue::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	int value = 0;
	bool hasValue = false;

	for(const auto& setting : settings) {
		if(setting.first == "value") {
			if(hasValue) {
				throw std::runtime_error("Multiple definition of attribute 'value'");
			}
			value = std::stoi(setting.second);
			hasValue = true;
		}
		else {
			throw std::runtime_error("Unknown parameter key=\"" + setting.first + "\" with value=\"" + setting.second + "\"");
		}
	}

	return std::unique_ptr<Object>(new IntValue(value));
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
