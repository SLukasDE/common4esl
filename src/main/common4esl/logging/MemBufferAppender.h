#ifndef COMMON4ESL_LOGGING_MEMBUFFERAPPENDER_H_
#define COMMON4ESL_LOGGING_MEMBUFFERAPPENDER_H_

#include <esl/logging/Appender.h>
#include <esl/logging/MemBufferAppender.h>

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

class MemBufferAppender: public esl::logging::Appender {
public:
	MemBufferAppender(esl::logging::MemBufferAppender::Settings settings);

	void setLayout(const esl::logging::Layout* aLayout) override;
	const esl::logging::Layout* getLayout() const override;

	/* both methods are NOT thread-safe */
	void setRecordLevel(esl::logging::Appender::RecordLevel aRecordLevel = RecordLevel::SELECTED) override;
	esl::logging::Appender::RecordLevel getRecordLevel() const override;

//protected:
	/* method is (currently) NOT thread-safe, but could be const */
	void flush(std::ostream* oStream) override;

	void write(const esl::logging::Streams::Location& location, const char* ptr, std::size_t size) override;

private:
	const esl::logging::Layout* layout = nullptr;
	esl::logging::Appender::RecordLevel recordLevel = esl::logging::Appender::RecordLevel::SELECTED;

    using LineBuffer = std::vector<char>;
    struct Entry {
    	Entry(std::size_t maxColumns)
    	: lineStaticSize((maxColumns > 0 ? maxColumns+1 : 0), 0)
    	{ }
    	esl::logging::Streams::Location location;
    	LineBuffer lineStaticSize;
    	std::string lineDynamicSize;
    };

    const esl::logging::MemBufferAppender::Settings settings;
    std::vector<Entry> entries;

	std::size_t rowProducer = 0;
	std::size_t rowConsumer = 0;
	std::size_t columnsProducer = 0;

	void write(const char* ptr, std::size_t size);
	void newline();
};

} /* namespace logging */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_LOGGING_MEMBUFFERAPPENDER_H_ */
