#include <esl/logging/MemBufferAppender.h>

#include <common4esl/logging/MemBufferAppender.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace logging {

MemBufferAppender::Settings::Settings(const std::vector<std::pair<std::string, std::string>>& settings) {
	bool hasMaxColumns = false;
	bool hasMaxLines = false;

	for(auto const& setting : settings) {
		if(setting.first == "max-lines") {
			if(hasMaxLines) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for MemBufferAppender");
			}
			hasMaxLines = true;
			maxLines = static_cast<std::size_t>(std::stoi(setting.second));
		}
		else if(setting.first == "max-columns") {
			if(hasMaxColumns) {
				throw std::runtime_error("Multiple definition of parameter key \"" + setting.first + "\" for MemBufferAppender");
			}
			hasMaxColumns = true;
			maxColumns = static_cast<std::size_t>(std::stoi(setting.second));
		}
		else {
			throw std::runtime_error("Invalid parameter key \"" + setting.first + "\" for MemBufferAppender");
		}
	}

	if(!hasMaxLines) {
		throw std::runtime_error("Missing definition of parameter key \"max-lines\" for MemBufferAppender");
	}
}

MemBufferAppender::MemBufferAppender(const Settings& settings)
: appender(new common4esl::logging::MemBufferAppender(settings))
{ }

std::unique_ptr<Appender> MemBufferAppender::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<Appender>(new MemBufferAppender(Settings(settings)));
}

void MemBufferAppender::setLayout(const Layout* aLayout) {
	appender->setLayout(aLayout);
}

const Layout* MemBufferAppender::getLayout() const {
	return appender->getLayout();
}

void MemBufferAppender::setRecordLevel(RecordLevel aRecordLevel) {
	appender->setRecordLevel(aRecordLevel);
}

Appender::RecordLevel MemBufferAppender::getRecordLevel() const {
	return appender->getRecordLevel();
}

void MemBufferAppender::flush(std::ostream* oStream) {
	appender->flush(oStream);
}

void MemBufferAppender::write(const Streams::Location& location, const char* ptr, std::size_t size) {
	appender->write(location, ptr, size);
}

} /* namespace logging */
} /* inline namespace v1_6 */
} /* namespace esl */
