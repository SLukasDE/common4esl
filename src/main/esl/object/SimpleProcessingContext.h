#ifndef ESL_OBJECT_SIMPLEPROCESSINGCONTEXT_H_
#define ESL_OBJECT_SIMPLEPROCESSINGCONTEXT_H_

#include <esl/com/common/server/RequestContext.h>
#include <esl/io/Input.h>
#include <esl/object/Context.h>
#include <esl/object/InitializeContext.h>
#include <esl/object/Object.h>
#include <esl/object/ProcessingContext.h>

#include <boost/filesystem/path.hpp>

#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace object {

class SimpleProcessingContext : public ProcessingContext, public InitializeContext {
public:
	struct Settings {
		Settings() = default;
		Settings(const std::vector<std::pair<std::string, std::string>>& settings);

		std::string exceptionHandlerId;
	};

	SimpleProcessingContext(const Settings& settings);

	static std::unique_ptr<ProcessingContext> create(const std::vector<std::pair<std::string, std::string>>& settings);


	void setParentObjectContext(Context* aParentObject) override;
	void addAlias(const std::string& destinationId, const std::string& sourceId) override;

	void addData(const std::string& configuration) override;
	void addFile(const boost::filesystem::path& filename) override;

	int getReturnCode() const override;

	void procedureRun(Context& context) override;
	void procedureCancel() override;

	void onEvent(const esl::object::Object& object) override;

	io::Input accept(com::common::server::RequestContext& requestContext) override;
	std::set<std::string> getNotifiers() const override;

	void addObject(const std::string& id, std::unique_ptr<Object> object) override;

	std::set<std::string> getObjectIds() const override;

	void initializeContext(Context& context) override;

protected:
	Object* findRawObject(const std::string& id) override;
	const Object* findRawObject(const std::string& id) const override;

private:
	SimpleProcessingContext(std::tuple<std::unique_ptr<Object>, ProcessingContext&, InitializeContext&> object);

	std::tuple<std::unique_ptr<Object>, ProcessingContext&, InitializeContext&> object;
};

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_OBJECT_SIMPLEPROCESSINGCONTEXT_H_ */
