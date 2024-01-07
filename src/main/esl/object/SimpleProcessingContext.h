#ifndef ESL_OBJECT_SIMPLEPROCESSINGCONTEXT_H_
#define ESL_OBJECT_SIMPLEPROCESSINGCONTEXT_H_

#include <esl/com/common/server/RequestContext.h>
#include <esl/io/Input.h>
#include <esl/object/Command.h>
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

class SimpleProcessingContextInterface : public ProcessingContext, public InitializeContext, public Command { };

class SimpleProcessingContext : public SimpleProcessingContextInterface {
public:
	struct Settings {
		Settings() = default;
		Settings(const std::vector<std::pair<std::string, std::string>>& settings);

		std::string exceptionHandlerId;
	};

	SimpleProcessingContext(const Settings& settings);

	static std::unique_ptr<ProcessingContext> create(const std::vector<std::pair<std::string, std::string>>& settings);
	static std::unique_ptr<SimpleProcessingContextInterface> createNative(const Settings& settings);


	void setParentObjectContext(Context* aParentObject) override;
	void addAlias(const std::string& destinationId, const std::string& sourceId) override;

	std::unique_ptr<Object> runCommand(const std::string& command, Object* argument) override;
	//void addData(const std::string& configuration) override;
	//void addFile(const boost::filesystem::path& filename) override;

	int getReturnCode() const override;

	void procedureRun(Context& context) override;
	void procedureCancel() override;

	io::Input accept(com::common::server::RequestContext& requestContext) override;
	std::set<std::string> getNotifiers() const override;

	void addObject(const std::string& id, std::unique_ptr<Object> object) override;

	std::set<std::string> getObjectIds() const override;

	void initializeContext(Context& context) override;

protected:
	Object* findRawObject(const std::string& id) override;
	const Object* findRawObject(const std::string& id) const override;

private:
	std::unique_ptr<SimpleProcessingContextInterface> object;
};

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_OBJECT_SIMPLEPROCESSINGCONTEXT_H_ */
