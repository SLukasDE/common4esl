#ifndef ESL_OBJECT_SETSTRINGVALUE_H_
#define ESL_OBJECT_SETSTRINGVALUE_H_

#include <esl/object/Object.h>
#include <esl/object/Value.h>

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace object {

class SetStringValue : public Value<std::set<std::string>> {
public:
	using Value<std::set<std::string>>::Value;

	static std::unique_ptr<Object> create(const std::vector<std::pair<std::string, std::string>>& settings);
};

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_OBJECT_SETSTRINGVALUE_H_ */
