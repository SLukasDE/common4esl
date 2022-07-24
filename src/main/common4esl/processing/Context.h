#ifndef COMMON4ESL_PROCESSING_CONTEXT_H_
#define COMMON4ESL_PROCESSING_CONTEXT_H_

#include <common4esl/processing/ContextEntry.h>

#include <esl/processing/Context.h>
#include <esl/logging/StreamReal.h>
#include <esl/logging/StreamEmpty.h>
#include <esl/logging/Location.h>
#include <esl/object/InitializeContext.h>
#include <esl/object/Object.h>
#include <esl/object/Context.h>
#include <esl/processing/Procedure.h>
#include <esl/system/Stacktrace.h>

#include <boost/filesystem/path.hpp>

#include <exception>
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

	enum HandleException {
		rethrow,
		stop,         // (add or override exception object)
		stopAndShow,  // (add or override exception object)
		ignore,       // (continue)
		ignoreAndShow // (continue)
	};
	bool hasHandleException = false;
	HandleException handleException = stop;

	bool hasShowStacktrace = false;
	bool showStacktrace = true;

	bool hasShowFilePosition = false;
	bool showFilePosition = true;

	struct ShowOutput {
		ShowOutput(std::ostream& aOStream)
		: ostream(&aOStream)
		{}

		ShowOutput(esl::logging::StreamReal& aStreamReal)
		: streamReal(&aStreamReal)
		{}

		ShowOutput(esl::logging::StreamEmpty& aStreamEmpty)
		: streamEmpty(&aStreamEmpty)
		{}

		std::ostream* ostream = nullptr;
		esl::logging::StreamReal* streamReal = nullptr;
		esl::logging::StreamEmpty* streamEmpty = nullptr;
	};
	std::unique_ptr<ShowOutput> showOutput;

	std::vector<std::unique_ptr<ContextEntry>> entries;

	struct IdElement {
		IdElement(std::unique_ptr<esl::object::Object> aObject)
		: object(std::move(aObject)),
		  refObject(*object),
		  initializeContext(dynamic_cast<esl::object::InitializeContext*>(&refObject))
		{ }

		IdElement(esl::object::Object& refObject)
		: refObject(refObject),
		  initializeContext(nullptr)
		{ }

		std::unique_ptr<esl::object::Object> object;
		esl::object::Object& refObject;
		esl::object::InitializeContext* initializeContext;
	};
	std::map<std::string, IdElement> objects;

	int returnCode = 0;
#if 0
	void printException(std::ostream& stream, int level, const std::exception& e, const std::string& plainException, const std::string& plainDetails, const esl::system::Stacktrace* stacktrace);
	void printException(esl::logging::StreamReal& stream, int level, const std::exception& e, const std::string& plainException, const std::string& plainDetails, const esl::system::Stacktrace* stacktrace, esl::logging::Location location);

	void printException(std::ostream& stream, std::exception_ptr exceptionPointer);
	void printException(esl::logging::StreamReal& stream, std::exception_ptr exceptionPointer, esl::logging::Location location = esl::logging::Location{});
#endif
};

} /* namespace processing */
} /* namespace common4esl */

#endif /* COMMON4ESL_PROCESSING_CONTEXT_H_ */
