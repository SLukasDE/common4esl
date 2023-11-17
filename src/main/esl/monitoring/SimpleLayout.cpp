#include <esl/monitoring/SimpleLayout.h>
#include <esl/utility/String.h>
#include <esl/system/Stacktrace.h>

#include <common4esl/monitoring/DefaultLayout.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace monitoring {

SimpleLayout::Settings::Settings(const std::vector<std::pair<std::string, std::string>>& settings) {
	bool hasShowTimestamp = false;
	bool hasShowLevel = false;
	bool hasShowTypename = false;
	bool hasShowAddress = false;
	bool hasShowFile = false;
	bool hasShowFunction = false;
	bool hasShowLineNo = false;
	bool hasShowThreadNo = false;

	for(auto const& setting : settings) {
		if(setting.first == "show-timestamp") {
			if(hasShowTimestamp) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-timestamp'."));
			}
			hasShowTimestamp = true;
			showTimestamp = esl::utility::String::toBool(setting.second);
		}
		else if(setting.first == "show-level") {
			if(hasShowLevel) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-level'."));
			}
			hasShowLevel = true;
			showLevel = esl::utility::String::toBool(setting.second);
		}
		else if(setting.first == "show-typename") {
			if(hasShowTypename) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-typename'."));
			}
			hasShowTypename = true;
			showTypename = esl::utility::String::toBool(setting.second);
		}
		else if(setting.first == "show-address") {
			if(hasShowAddress) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-address'."));
			}
			hasShowAddress = true;
			showAddress = esl::utility::String::toBool(setting.second);
		}
		else if(setting.first == "show-file") {
			if(hasShowFile) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-file'."));
			}
			hasShowFile = true;
			showFile = esl::utility::String::toBool(setting.second);
		}
		else if(setting.first == "show-function") {
			if(hasShowFunction) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-function'."));
			}
			hasShowFunction = true;
			showFunction = esl::utility::String::toBool(setting.second);
		}
		else if(setting.first == "show-line-no") {
			if(hasShowLineNo) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-line-no'."));
			}
			hasShowLineNo = true;
			showLineNo = esl::utility::String::toBool(setting.second);
		}
		else if(setting.first == "show-thread-no") {
			if(hasShowThreadNo) {
	            throw esl::system::Stacktrace::add(std::runtime_error("multiple definition of attribute 'show-thread-no'."));
			}
			hasShowThreadNo = true;
			showThreadNo = esl::utility::String::toBool(setting.second);
		}
		else {
			throw std::runtime_error("Unknown key \"" + setting.first + "\"");
		}
	}
}

SimpleLayout::SimpleLayout(const Settings& settings)
: layout(new common4esl::monitoring::DefaultLayout(settings))
{ }

std::unique_ptr<Layout> SimpleLayout::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<Layout>(new SimpleLayout(Settings(settings)));
}

std::string SimpleLayout::toString(const Streams::Location& location) const {
	return layout->toString(location);
}

} /* namespace monitoring */
} /* inline namespace v1_6 */
} /* namespace esl */
