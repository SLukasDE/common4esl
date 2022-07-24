#ifndef COMMON4ESL_CONFIG_CONTEXT_BASICCLIENT_H_
#define COMMON4ESL_CONFIG_CONTEXT_BASICCLIENT_H_

#include <common4esl/config/Config.h>
#include <common4esl/config/Setting.h>
#include <common4esl/processing/Context.h>

#include <esl/object/Object.h>

#include <tinyxml2/tinyxml2.h>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace common4esl {
namespace config {
namespace context {

class BasicClient : public Config {
public:
	BasicClient(const std::string& fileName, const tinyxml2::XMLElement& element);

	void save(std::ostream& oStream, std::size_t spaces) const;
	void install(processing::Context& context) const;

private:
	std::string id;
	std::string implementation;
	std::string refId;
	std::vector<Setting> settings;

	std::unique_ptr<esl::object::Object> create() const;
	void parseInnerElement(const tinyxml2::XMLElement& element);
};

} /* namespace context */
} /* namespace config */
} /* namespace common4esl */

#endif /* COMMON4ESL_CONFIG_CONTEXT_BASICCLIENT_H_ */
