#ifndef COMMON4ESL_MONITORING_OSTREAMAPPENDER_H_
#define COMMON4ESL_MONITORING_OSTREAMAPPENDER_H_

#include <esl/monitoring/Appender.h>
#include <esl/monitoring/OStreamAppender.h>

#include <esl/monitoring/Layout.h>
#include <esl/monitoring/Streams.h>

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
inline namespace v1_6 {
namespace monitoring {

class OStreamAppender : public esl::monitoring::Appender {
public:
	OStreamAppender(const esl::monitoring::OStreamAppender::Settings& settings);

	void setLayout(const esl::monitoring::Layout* aLayout) override;
	const esl::monitoring::Layout* getLayout() const override;

	/* both methods are NOT thread-safe */
	void setRecordLevel(esl::monitoring::Appender::RecordLevel aRecordLevel = RecordLevel::SELECTED) override;
	esl::monitoring::Appender::RecordLevel getRecordLevel() const override;

	void flush(std::ostream*) override;

	void write(const esl::monitoring::Streams::Location& location, const char* ptr, std::size_t size) override;

private:
	const esl::monitoring::Layout* layout = nullptr;
	esl::monitoring::Appender::RecordLevel recordLevel = RecordLevel::SELECTED;

	bool isFirstCharacterInLine = true;
	esl::monitoring::Streams::Location lastLocation;
    const esl::monitoring::OStreamAppender::Settings settings;

	std::ostream& getOStream(esl::monitoring::Streams::Level level);
};

} /* namespace monitoring */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_MONITORING_OSTREAMAPPENDER_H_ */
