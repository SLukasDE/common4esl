#ifndef COMMON4ESL_OBJECT_CONTEXT_H_
#define COMMON4ESL_OBJECT_CONTEXT_H_

#include <esl/object/Context.h>
#include <esl/object/Object.h>

#include <map>
#include <memory>
#include <set>
#include <string>

namespace common4esl {
namespace object {

class Context : public esl::object::Context {
public:
	std::set<std::string> getObjectIds() const override;

protected:
	void addRawObject(const std::string& id, std::unique_ptr<esl::object::Object> object) override;
	esl::object::Object* findRawObject(const std::string& id) override;
	const esl::object::Object* findRawObject(const std::string& id) const override;

private:
	std::map<std::string, std::unique_ptr<esl::object::Object>> objects;
};

} /* namespace object */
} /* namespace common4esl */

#endif /* COMMON4ESL_OBJECT_CONTEXT_H_ */
