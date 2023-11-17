/*
MIT License
Copyright (c) 2019-2022 Sven Lukas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <common4esl/monitoring/MemBufferAppender.h>

namespace common4esl {
inline namespace v1_6 {
namespace monitoring {

namespace {
bool equal(const esl::monitoring::Streams::Location& location1, const esl::monitoring::Streams::Location& location2) {
	return location1.level == location2.level &&
			location1.object == location2.object &&
			location1.typeName == location2.typeName &&
			location1.function == location2.function &&
			location1.file == location2.file &&
			location1.threadId == location2.threadId;
}
}

MemBufferAppender::MemBufferAppender(esl::monitoring::MemBufferAppender::Settings aSettings)
: settings(aSettings),
  entries(settings.maxLines+1, settings.maxColumns)
{

}

void MemBufferAppender::setLayout(const esl::monitoring::Layout* aLayout) {
	layout = aLayout;
}

const esl::monitoring::Layout* MemBufferAppender::getLayout() const {
	return layout;
}

/* both methods are NOT thread-safe */
void MemBufferAppender::setRecordLevel(esl::monitoring::Appender::RecordLevel aRecordLevel) {
	recordLevel = aRecordLevel;
}

esl::monitoring::Appender::RecordLevel MemBufferAppender::getRecordLevel() const {
	return recordLevel;
}

void MemBufferAppender::flush(std::ostream* oStream) {
	if(!oStream) {
		return;
	}

	std::vector<std::tuple<esl::monitoring::Streams::Location, std::string>> buffer;

	if(settings.maxColumns > 0) {
		for(std::size_t tmpIdxCons = rowConsumer; tmpIdxCons != rowProducer; tmpIdxCons = (tmpIdxCons + 1) % (settings.maxLines+1)) {
			buffer.push_back(std::make_tuple(entries[tmpIdxCons].location, std::string(&entries[tmpIdxCons].lineStaticSize[0])));
		}
	}
	else {
		for(std::size_t tmpIdxCons = rowConsumer; tmpIdxCons != rowProducer; tmpIdxCons = (tmpIdxCons + 1) % (settings.maxLines+1)) {
			buffer.push_back(std::make_tuple(entries[tmpIdxCons].location, entries[tmpIdxCons].lineDynamicSize));
		}
	}

    if(layout) {
        for(const auto& entry : buffer) {
        	*oStream << layout->toString(std::get<0>(entry)) << std::get<1>(entry) << "\n";
        }
    }
    else {
        for(const auto& entry : buffer) {
        	*oStream << std::get<1>(entry) << "\n";
        }
    }
}

void MemBufferAppender::write(const esl::monitoring::Streams::Location& location, const char* str, std::size_t len) {
	switch(getRecordLevel()) {
	case RecordLevel::OFF:
		return;
	case RecordLevel::ALL:
		break;
	default: /* RecordLevel::SELECTED */
		if(!location.enabled) {
			return;
		}
		break;
	}

	if(!equal(entries[rowProducer].location, location)) {
		if(columnsProducer > 0) {
			newline();
		}
    	entries[rowProducer].location = location;
	}

	if(len > 0) {
		const char* begin = str;

		for(auto iter = str; iter != &str[len]; ++iter) {
			if(*iter == '\n') {
	            write(begin, iter-begin);
	            newline();
	        	entries[rowProducer].location = location;
				begin = iter+1;
			}
		}

		if(begin != &str[len]) {
            write(begin, &str[len]-begin);
		}
	}
}

void MemBufferAppender::write(const char* ptr, std::size_t size) {
    if(settings.maxColumns == 0) {
    	entries[rowProducer].lineDynamicSize += std::string(ptr, size);
        columnsProducer += size;
    }
    else {
        std::strncat(&entries[rowProducer].lineStaticSize[columnsProducer], ptr, std::min(size, settings.maxColumns - columnsProducer));
        columnsProducer += std::min(size, settings.maxColumns - columnsProducer);
    }
}

void MemBufferAppender::newline() {
    rowProducer = (rowProducer + 1) % (settings.maxLines + 1);
    if(rowConsumer == rowProducer) {
        rowConsumer = (rowConsumer + 1) % (settings.maxLines + 1);
    }

    if(settings.maxColumns == 0) {
    	entries[rowProducer].lineDynamicSize.clear();
    }
    else {
    	entries[rowProducer].lineStaticSize[0] = 0;
    }
    columnsProducer = 0;
}

} /* namespace monitoring */
} /* inline namespace v1_6 */
} /* namespace common4esl */
