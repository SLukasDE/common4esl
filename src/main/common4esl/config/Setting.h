#ifndef COMMON4ESL_CONFIG_SETTING_H_
#define COMMON4ESL_CONFIG_SETTING_H_

#include <common4esl/config/Config.h>

#include <tinyxml2.h>

#include <string>
#include <ostream>

namespace common4esl {
inline namespace v1_6 {
namespace config {

class Setting : public Config {
public:
	Setting(const std::string& fileName, const tinyxml2::XMLElement& element, bool allowLanguage);

	void saveParameter(std::ostream& oStream, std::size_t spaces) const;

	std::string key;
	std::string value;
	std::string language;
};

} /* namespace config */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_CONFIG_SETTING_H_ */
