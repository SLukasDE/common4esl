#ifndef COMMON4ESL_CONFIG_CONFIG_H_
#define COMMON4ESL_CONFIG_CONFIG_H_

#include <tinyxml2.h>

#include <string>
#include <utility>

namespace common4esl {
inline namespace v1_6 {
namespace config {

class Config {
public:
	Config() = delete;
	Config(const std::string& filename);
	Config(const std::string& filename, const tinyxml2::XMLElement& element);
	virtual ~Config() = default;

	std::string evaluate(const std::string& expression, const std::string& language) const;

	const std::string& getFileName() const noexcept;
	int getLineNo() const noexcept;
	std::pair<std::string, int> getXMLFile() const noexcept;

protected:
	std::pair<std::string, int> setXMLFile(const std::string& filename, int lineNo);
	std::pair<std::string, int> setXMLFile(const std::string& filename, const tinyxml2::XMLElement& element);
	std::pair<std::string, int> setXMLFile(const std::pair<std::string, int>& xmlFile);

	std::string makeSpaces(std::size_t spaces) const;
	static bool stringToBool(bool& b, std::string str);

private:
	std::string filename;
	int lineNo;
};

} /* namespace config */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_CONFIG_CONFIG_H_ */
