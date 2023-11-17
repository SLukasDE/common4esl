#ifndef ESL_OBJECT_VECTORPAIRSTRINGSTRINGVALUE_H_
#define ESL_OBJECT_VECTORPAIRSTRINGSTRINGVALUE_H_

#include <esl/object/Object.h>
#include <esl/object/Value.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace object {

class VectorPairStringStringValue : public Value<std::vector<std::pair<std::string, std::string>>> {
public:
	using Value<std::vector<std::pair<std::string, std::string>>>::Value;

	static std::unique_ptr<Object> create(const std::vector<std::pair<std::string, std::string>>& settings);
};

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_OBJECT_VECTORPAIRSTRINGSTRINGVALUE_H_ */
