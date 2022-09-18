#ifndef COMMON4ESL_PROCESSING_CONTEXT_H_
#define COMMON4ESL_PROCESSING_CONTEXT_H_

#include <common4esl/processing/ContextEntry.h>

#include <esl/processing/Context.h>
#include <esl/object/InitializeContext.h>
#include <esl/object/Object.h>
#include <esl/object/Context.h>
#include <esl/processing/Procedure.h>

#include <boost/filesystem/path.hpp>

#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>


namespace common4esl {
namespace processing {

class Context : public esl::processing::Context, public esl::object::InitializeContext  {
public:
	static std::unique_ptr<esl::processing::Context> create(const std::vector<std::pair<std::string, std::string>>& settings);

	Context(const std::vector<std::pair<std::string, std::string>>& settings);

	void setParent(Context* parentContext);

	esl::processing::Context& addData(const std::string& configuration) override;
	esl::processing::Context& addFile(const boost::filesystem::path& filename) override;
	esl::processing::Context& addReference(const std::string& destinationId, const std::string& sourceId) override;
	int getReturnCode() const override;

	void onEvent(const esl::object::Object& object) override;
	std::set<std::string> getObjectIds() const override;
	void procedureRun(esl::object::Context& context) override;

	void initializeContext(esl::object::Context&) override;

protected:
	esl::object::Object* findRawObject(const std::string& id) override;
	const esl::object::Object* findRawObject(const std::string& id) const override;
	void addRawObject(const std::string& id, std::unique_ptr<esl::object::Object> object) override;

private:
	Context* parent = nullptr;

	bool isInitialized = false;

	std::string exceptionHandlerId;
	esl::processing::Procedure* exceptionHandler = nullptr;

	std::vector<std::unique_ptr<ContextEntry>> entries;

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

} /* namespace processing */
} /* namespace common4esl */

#endif /* COMMON4ESL_PROCESSING_CONTEXT_H_ */
