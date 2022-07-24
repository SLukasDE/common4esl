#ifndef COMMON4ESL_LOGGING_OSTREAMAPPENDER_H_
#define COMMON4ESL_LOGGING_OSTREAMAPPENDER_H_

#include <esl/logging/Appender.h>
#include <esl/logging/Layout.h>
#include <esl/logging/Level.h>
#include <esl/logging/Location.h>

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
namespace logging {

class OStreamAppender : public esl::logging::Appender {
public:
	static std::unique_ptr<esl::logging::Appender> create(const std::vector<std::pair<std::string, std::string>>& settings);

	OStreamAppender(const std::vector<std::pair<std::string, std::string>>& settings);

	void setLayout(const esl::logging::Layout* aLayout) override;
	const esl::logging::Layout* getLayout() const override;

	/* both methods are NOT thread-safe */
	void setRecordLevel(esl::logging::Appender::RecordLevel aRecordLevel = RecordLevel::SELECTED) override;
	esl::logging::Appender::RecordLevel getRecordLevel() const override;

	void flush(std::ostream*) override;

	void write(const esl::logging::Location& location, const char* ptr, std::size_t size) override;

	struct OStreams {
		OStreams(std::ostream& trace, std::ostream& debug, std::ostream& info, std::ostream& warn, std::ostream& error);
		std::ostream& trace;
		std::ostream& debug;
		std::ostream& info;
		std::ostream& warn;
		std::ostream& error;
	};

private:
	const esl::logging::Layout* layout = nullptr;
	esl::logging::Appender::RecordLevel recordLevel = RecordLevel::SELECTED;

	bool isFirstCharacterInLine = true;
	esl::logging::Location lastLocation;
	OStreams oStreams;

	std::ostream& getOStream(esl::logging::Level level);
};

} /* namespace logging */
} /* namespace common4esl */

#endif /* COMMON4ESL_LOGGING_OSTREAMAPPENDER_H_ */
