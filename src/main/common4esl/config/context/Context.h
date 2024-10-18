#ifndef COMMON4ESL_CONFIG_CONTEXT_CONTEXT_H_
#define COMMON4ESL_CONFIG_CONTEXT_CONTEXT_H_

#include <common4esl/config/Config.h>
#include <common4esl/config/context/Entry.h>
#include <common4esl/config/Setting.h>
#include <common4esl/object/ProcessingContext.h>
//#include <common4esl/processing/Context.h>

#include <esl/object/Object.h>
#include <esl/monitoring/Appender.h>
#include <esl/monitoring/Layout.h>
#include <esl/plugin/Library.h>

#include <tinyxml2.h>

#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
inline namespace v1_6 {
namespace config {
namespace context {

class Context : public Config {
public:
	Context(const std::string& filename, const tinyxml2::XMLElement& element);
	Context(bool isFile, const std::string& value);

	void save(std::ostream& oStream, std::size_t spaces = 0) const;
	void install(object::ProcessingContext& context);
	void loadLibraries();

private:
	bool isRoot = false;

	std::string id;
	std::string implementation;
	std::string refId;
	std::vector<Setting> settings;

	tinyxml2::XMLDocument xmlDocument;

	std::vector<std::pair<std::string, std::string>> libraries;
	std::vector<std::unique_ptr<Entry>> entries;

	std::set<std::string> filesLoaded;

	std::unique_ptr<esl::object::Object> create() const;
	void parseInnerElement(const tinyxml2::XMLElement& element);

	void loadXML(const tinyxml2::XMLElement& element);
	void parseInclude(const tinyxml2::XMLElement& element);
	void parseLibrary(const tinyxml2::XMLElement& element);
};

} /* namespace context */
} /* namespace config */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_CONFIG_CONTEXT_CONTEXT_H_ */
