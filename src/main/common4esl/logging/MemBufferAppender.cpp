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

#include <common4esl/logging/MemBufferAppender.h>

namespace common4esl {
namespace logging {

namespace {
MemBufferAppender::Dimensions getDimensions(const std::vector<std::pair<std::string, std::string>>& settings) {
	std::size_t maxColumns = 0;
	bool maxColumnsDefined = false;

	std::size_t maxRows;
	bool maxRowsDefined = false;

	for(auto const& setting : settings) {
		if(setting.first == "max-lines") {
			if(maxRowsDefined) {
				throw std::runtime_error("Multiple definition of parameter key \"max-rows\" for MemBuffer appender");
			}
			maxRowsDefined = true;
			maxRows = static_cast<std::size_t>(std::stoi(setting.second));
		}
		else if(setting.first == "max-columns") {
			if(maxColumnsDefined) {
				throw std::runtime_error("Multiple definition of parameter key \"max-rows\" for MemBuffer appender");
			}
			maxColumnsDefined = true;
			maxColumns = static_cast<std::size_t>(std::stoi(setting.second));
		}
		else {
			throw std::runtime_error("Invalid parameter key \"" + setting.first + "\" for MemBuffer appender");
		}
	}

	if(!maxRowsDefined) {
		throw std::runtime_error("Missing definition of parameter key \"max-rows\" for MemBuffer appender");
	}

	return MemBufferAppender::Dimensions(maxRows + 1, maxColumns);
}

}

MemBufferAppender::Dimensions::Dimensions(const std::size_t aRows, const std::size_t aColumns)
: rows(aRows),
  columns(aColumns)
{ }

std::unique_ptr<esl::logging::Appender> MemBufferAppender::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::logging::Appender>(new MemBufferAppender(settings));
}

MemBufferAppender::MemBufferAppender(const std::vector<std::pair<std::string, std::string>>& settings)
: dimensions(getDimensions(settings)),
  entries(dimensions.rows, (dimensions.columns > 0 ? dimensions.columns : 0))
{ }

void MemBufferAppender::setLayout(const esl::logging::Layout* aLayout) {
	layout = aLayout;
}

const esl::logging::Layout* MemBufferAppender::getLayout() const {
	return layout;
}

/* both methods are NOT thread-safe */
void MemBufferAppender::setRecordLevel(esl::logging::Appender::RecordLevel aRecordLevel) {
	recordLevel = aRecordLevel;
}

esl::logging::Appender::RecordLevel MemBufferAppender::getRecordLevel() const {
	return recordLevel;
}

void MemBufferAppender::flush(std::ostream* oStream) {
	if(!oStream) {
		return;
	}

	std::vector<std::tuple<esl::logging::Location, std::string>> buffer;

	if(dimensions.columns > 0) {
		for(std::size_t tmpIdxCons = rowConsumer; tmpIdxCons != rowProducer; tmpIdxCons = (tmpIdxCons + 1) % dimensions.rows) {
			buffer.push_back(std::make_tuple(entries[tmpIdxCons].location, std::string(&entries[tmpIdxCons].lineStaticSize[0])));
		}
	}
	else {
		for(std::size_t tmpIdxCons = rowConsumer; tmpIdxCons != rowProducer; tmpIdxCons = (tmpIdxCons + 1) % dimensions.rows) {
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

void MemBufferAppender::write(const esl::logging::Location& location, const char* str, std::size_t len) {
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

	if(entries[rowProducer].location != location) {
		if(columnsProducer > 0) {
			newline();
		}
    	entries[rowProducer].location = location;
	}

	const char* begin = str;

	for(auto iter = str; iter != &str[len]; ++iter) {
		if(*iter == '\n') {
            write(begin, iter-begin);
            newline();
        	entries[rowProducer].location = location;
			begin = iter+1;
		}
	}
}

void MemBufferAppender::write(const char* ptr, std::size_t size) {
    if(dimensions.columns == 0) {
    	entries[rowProducer].lineDynamicSize += std::string(ptr, size);
        columnsProducer += size;
    }
    else {
        std::strncat(&entries[rowProducer].lineStaticSize[columnsProducer], ptr, std::min(size, dimensions.columns - columnsProducer));
        columnsProducer += std::min(size, dimensions.columns - columnsProducer);
    }
}

void MemBufferAppender::newline() {
    rowProducer = (rowProducer + 1) % dimensions.rows;
    if(rowConsumer == rowProducer) {
        rowConsumer = (rowConsumer + 1) % dimensions.rows;
    }

    if(dimensions.columns == 0) {
    	entries[rowProducer].lineDynamicSize.clear();
    }
    else {
    	entries[rowProducer].lineStaticSize[0] = 0;
    }
    columnsProducer = 0;
}

} /* namespace logging */
} /* namespace common4esl */
