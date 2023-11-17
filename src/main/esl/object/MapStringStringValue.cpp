#include <esl/object/MapStringStringValue.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

std::unique_ptr<Object> MapStringStringValue::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	std::map<std::string, std::string> value;

	for(const auto& setting : settings) {
		if(value.count(setting.first) > 0) {
			throw std::runtime_error("Multiple definition of attribute '" + setting.first + "'");
		}
		value.insert(std::make_pair(setting.first, setting.second));
	}

	return std::unique_ptr<Object>(new MapStringStringValue(std::move(value)));
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
