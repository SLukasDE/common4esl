#ifndef ESL_LOGGING_OSTREAMAPPENDER_H_
#define ESL_LOGGING_OSTREAMAPPENDER_H_

#include <esl/logging/Appender.h>
#include <esl/logging/Layout.h>
#include <esl/logging/Streams.h>

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace logging {

class OStreamAppender : public Appender {
public:
	struct Settings {
		Settings(const std::vector<std::pair<std::string, std::string>>& settings);
		Settings(std::ostream& trace, std::ostream& debug, std::ostream& info, std::ostream& warn, std::ostream& error);

		std::ostream& trace;
		std::ostream& debug;
		std::ostream& info;
		std::ostream& warn;
		std::ostream& error;
	};

	OStreamAppender(const Settings& settings);

	static std::unique_ptr<Appender> create(const std::vector<std::pair<std::string, std::string>>& settings);

	void setLayout(const Layout* aLayout) override;
	const Layout* getLayout() const override;

	/* both methods are NOT thread-safe */
	void setRecordLevel(RecordLevel aRecordLevel = RecordLevel::SELECTED) override;
	RecordLevel getRecordLevel() const override;

	void flush(std::ostream* oStream) override;

	void write(const Streams::Location& location, const char* ptr, std::size_t size) override;

private:
	std::unique_ptr<Appender> appender;
};

} /* namespace logging */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_LOGGING_OSTREAMAPPENDER_H_ */
