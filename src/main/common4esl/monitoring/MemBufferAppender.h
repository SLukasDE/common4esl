#ifndef COMMON4ESL_MONITORING_MEMBUFFERAPPENDER_H_
#define COMMON4ESL_MONITORING_MEMBUFFERAPPENDER_H_

#include <esl/monitoring/Appender.h>
#include <esl/monitoring/MemBufferAppender.h>

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

class MemBufferAppender: public esl::monitoring::Appender {
public:
	MemBufferAppender(esl::monitoring::MemBufferAppender::Settings settings);

	void setLayout(const esl::monitoring::Layout* aLayout) override;
	const esl::monitoring::Layout* getLayout() const override;

	/* both methods are NOT thread-safe */
	void setRecordLevel(esl::monitoring::Appender::RecordLevel aRecordLevel = RecordLevel::SELECTED) override;
	esl::monitoring::Appender::RecordLevel getRecordLevel() const override;

//protected:
	/* method is (currently) NOT thread-safe, but could be const */
	void flush(std::ostream* oStream) override;

	void write(const esl::monitoring::Streams::Location& location, const char* ptr, std::size_t size) override;

private:
	const esl::monitoring::Layout* layout = nullptr;
	esl::monitoring::Appender::RecordLevel recordLevel = esl::monitoring::Appender::RecordLevel::SELECTED;

    using LineBuffer = std::vector<char>;
    struct Entry {
    	Entry(std::size_t maxColumns)
    	: lineStaticSize((maxColumns > 0 ? maxColumns+1 : 0), 0)
    	{ }
    	esl::monitoring::Streams::Location location;
    	LineBuffer lineStaticSize;
    	std::string lineDynamicSize;
    };

    const esl::monitoring::MemBufferAppender::Settings settings;
    std::vector<Entry> entries;

	std::size_t rowProducer = 0;
	std::size_t rowConsumer = 0;
	std::size_t columnsProducer = 0;

	void write(const char* ptr, std::size_t size);
	void newline();
};

} /* namespace monitoring */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_MONITORING_MEMBUFFERAPPENDER_H_ */
