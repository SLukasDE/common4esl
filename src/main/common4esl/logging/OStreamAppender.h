#ifndef COMMON4ESL_LOGGING_OSTREAMAPPENDER_H_
#define COMMON4ESL_LOGGING_OSTREAMAPPENDER_H_

#include <esl/logging/Appender.h>
#include <esl/logging/OStreamAppender.h>

#include <esl/logging/Layout.h>
#include <esl/logging/Streams.h>

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
inline namespace v1_6 {
namespace logging {

class OStreamAppender : public esl::logging::Appender {
public:
	OStreamAppender(const esl::logging::OStreamAppender::Settings& settings);

	void setLayout(const esl::logging::Layout* aLayout) override;
	const esl::logging::Layout* getLayout() const override;

	/* both methods are NOT thread-safe */
	void setRecordLevel(esl::logging::Appender::RecordLevel aRecordLevel = RecordLevel::SELECTED) override;
	esl::logging::Appender::RecordLevel getRecordLevel() const override;

	void flush(std::ostream*) override;

	void write(const esl::logging::Streams::Location& location, const char* ptr, std::size_t size) override;

private:
	const esl::logging::Layout* layout = nullptr;
	esl::logging::Appender::RecordLevel recordLevel = RecordLevel::SELECTED;

	bool isFirstCharacterInLine = true;
	esl::logging::Streams::Location lastLocation;
    const esl::logging::OStreamAppender::Settings settings;

	std::ostream& getOStream(esl::logging::Streams::Level level);
};

} /* namespace logging */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_LOGGING_OSTREAMAPPENDER_H_ */
