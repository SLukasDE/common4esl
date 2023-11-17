#include <esl/object/VectorPairStringStringValue.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

std::unique_ptr<Object> VectorPairStringStringValue::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<Object>(new VectorPairStringStringValue(settings));
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
