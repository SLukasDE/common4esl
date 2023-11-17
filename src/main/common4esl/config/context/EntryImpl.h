#ifndef COMMON4ESL_CONFIG_CONTEXT_ENTRYIMPL_H_
#define COMMON4ESL_CONFIG_CONTEXT_ENTRYIMPL_H_

#include <common4esl/config/Config.h>
#include <common4esl/config/context/BasicClient.h>
#include <common4esl/config/context/BasicServer.h>
#include <common4esl/config/context/Database.h>
#include <common4esl/config/context/Entry.h>
#include <common4esl/config/context/HttpClient.h>
#include <common4esl/config/context/HttpServer.h>
#include <common4esl/config/context/Object.h>
#include <common4esl/config/context/Procedure.h>
#include <common4esl/object/ProcessingContext.h>

#include <memory>
#include <ostream>

#include <tinyxml2/tinyxml2.h>

namespace common4esl {
inline namespace v1_6 {
namespace config {
namespace context {

class EntryImpl : public Entry {
public:
	EntryImpl(const std::string& fileName, const tinyxml2::XMLElement& element);

	void save(std::ostream& oStream, std::size_t spaces) const override;
	void install(object::ProcessingContext& context) const override;

private:
	std::unique_ptr<Object> object;
	std::unique_ptr<Database> database;

	std::unique_ptr<Procedure> procedure;

	std::unique_ptr<BasicClient> basicClient;
	std::unique_ptr<BasicServer> basicServer;

	std::unique_ptr<HttpClient> httpClient;
	std::unique_ptr<HttpServer> httpServer;
};

} /* namespace context */
} /* namespace config */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_CONFIG_CONTEXT_ENTRYIMPL_H_ */
