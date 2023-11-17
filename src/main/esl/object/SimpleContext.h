#ifndef ESL_OBJECT_SIMPLECONTEXT_H_
#define ESL_OBJECT_SIMPLECONTEXT_H_

#include <esl/object/Context.h>
#include <esl/object/Object.h>

#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace object {

class SimpleContext : public Context {
public:
	struct Settings {
		Settings() = default;
		Settings(const std::vector<std::pair<std::string, std::string>>& settings);
	};

	SimpleContext();
	SimpleContext(const Settings& settings);

	static std::unique_ptr<Context> create(const std::vector<std::pair<std::string, std::string>>& settings);

	void addObject(const std::string& id, std::unique_ptr<Object> object) override;
	std::set<std::string> getObjectIds() const override;

protected:
	Object* findRawObject(const std::string& id) override;
	const Object* findRawObject(const std::string& id) const override;

private:
	std::unique_ptr<Context> context;
};

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_OBJECT_SIMPLECONTEXT_H_ */
