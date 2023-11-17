#include <esl/object/StringValue.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

std::unique_ptr<Object> StringValue::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	std::string value;
	bool hasValue = false;

	for(const auto& setting : settings) {
		if(setting.first == "value") {
			if(hasValue) {
				throw std::runtime_error("Multiple definition of attribute 'value'");
			}
			value = setting.second;
			hasValue = true;
		}
		else {
			throw std::runtime_error("Unknown parameter key=\"" + setting.first + "\" with value=\"" + setting.second + "\"");
		}
	}

	return std::unique_ptr<Object>(new StringValue(std::move(value)));
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
