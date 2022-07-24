#ifndef COMMON4ESL_LOGGING_DEFAULTLAYOUT_H_
#define COMMON4ESL_LOGGING_DEFAULTLAYOUT_H_

#include <esl/logging/Layout.h>
#include <esl/logging/Location.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
namespace logging {

class DefaultLayout : public esl::logging::Layout {
public:
	static std::unique_ptr<esl::logging::Layout> create(const std::vector<std::pair<std::string, std::string>>& settings);

	DefaultLayout(const std::vector<std::pair<std::string, std::string>>& values);

	std::string toString(const esl::logging::Location& location) const override;

	bool getShowTimestamp() const;
	bool getShowLevel() const;
	bool getShowTypename() const;
	bool getShowAddress() const;
	bool getShowFile() const;
	bool getShowFunction() const;
	bool getShowLineNo() const;
	bool getShowThreadNo() const;

private:
	bool showTimestamp = true;
	bool showLevel = true;
	bool showTypename = true;
	bool showAddress = true;
	bool showFile = false;
	bool showFunction = false;
	bool showLineNo = false;
	bool showThreadNo = true;
};

} /* namespace logging */
} /* namespace common4esl */

#endif /* COMMON4ESL_LOGGING_DEFAULTLAYOUT_H_ */
