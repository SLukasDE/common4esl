#ifndef COMMON4ESL_OBJECT_PROCESSINGCONTEXT_H_
#define COMMON4ESL_OBJECT_PROCESSINGCONTEXT_H_

#include <common4esl/object/ProcessingContextEntry.h>

#include <esl/com/common/server/RequestContext.h>
#include <esl/io/Input.h>
#include <esl/object/Context.h>
#include <esl/object/InitializeContext.h>
#include <esl/object/Object.h>
#include <esl/object/Procedure.h>
#include <esl/object/ProcessingContext.h>
#include <esl/object/SimpleProcessingContext.h>

#include <boost/filesystem/path.hpp>

#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>


namespace common4esl {
inline namespace v1_6 {
namespace object {

class ProcessingContext : public esl::object::ProcessingContext, public esl::object::InitializeContext  {
public:
	ProcessingContext(const esl::object::SimpleProcessingContext::Settings& settings);

	void setParentObjectContext(esl::object::Context* aParentObject);
	void setParent(Context* parentContext);

	void addObject(const std::string& id, std::unique_ptr<esl::object::Object> object) override;
	void addData(const std::string& configuration) override;
	void addFile(const boost::filesystem::path& filename) override;
	void addAlias(const std::string& destinationId, const std::string& sourceId) override;
	int getReturnCode() const override;

	std::set<std::string> getObjectIds() const override;
	void procedureRun(esl::object::Context& context) override;

	esl::io::Input accept(esl::com::common::server::RequestContext& requestContext) override;
	std::set<std::string> getNotifiers() const override;

	void initializeContext(esl::object::Context&) override;

protected:
	esl::object::Object* findRawObject(const std::string& id) override;
	const esl::object::Object* findRawObject(const std::string& id) const override;

private:
	Context* parent = nullptr;

	bool isInitialized = false;

	esl::object::SimpleProcessingContext::Settings settings;
	esl::object::Procedure* exceptionHandler = nullptr;

	std::vector<std::unique_ptr<ProcessingContextEntry>> entries;

	struct IdElement {
		IdElement(std::unique_ptr<esl::object::Object> aObject);
		IdElement(esl::object::Object& refObject);

		std::unique_ptr<esl::object::Object> object;
		esl::object::Object& refObject;
		esl::object::InitializeContext* initializeContext;
	};
	std::map<std::string, IdElement> objects;

	int returnCode = 0;
};

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_OBJECT_PROCESSINGCONTEXT_H_ */
