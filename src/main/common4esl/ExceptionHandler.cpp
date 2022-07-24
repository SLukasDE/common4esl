#include <common4esl/ExceptionHandler.h>
#include <common4esl/HttpStatusCode.h>

#include <esl/com/http/server/exception/StatusCode.h>
#include <esl/database/exception/SqlError.h>
#include <esl/io/FilePosition.h>
#include <esl/plugin/exception/PluginNotFound.h>
#include <esl/plugin/Registry.h>
#include <esl/system/Stacktrace.h>

#include <memory>
#include <sstream>
#include <stdexcept>

namespace common4esl {
namespace {
std::string createFileMessage(const esl::io::FilePosition* filePosition) {
	return filePosition ? "file \"" + filePosition->getFileName() + "\", line " + std::to_string(filePosition->getLineNo()) : "";
}

std::string createStackstrace(const esl::system::Stacktrace* stacktrace) {
	std::stringstream sstream;
	if(stacktrace) {
		stacktrace->dump(sstream);
	}
	return sstream.str();
}
} /* anonymous namespace */

ExceptionHandler::ExceptionHandler(std::exception_ptr exceptionPointer, bool aShowStacktrace, bool aShowFilePosition)
: showStacktrace(aShowStacktrace),
  showFilePosition(aShowFilePosition)
{
	initialize(exceptionPointer);
}

void ExceptionHandler::dump(std::ostream& stream) const {
	for(std::size_t i=0; i<entries.size(); ++i) {
		stream << "[" << (i+1) << "] Exception : " << entries[i].exceptionType << "\n";
		stream << "[" << (i+1) << "] What      : " << entries[i].what << "\n";

		if(!entries[i].details.empty()) {
			stream << "[" << (i+1) << "] Details   : " << entries[i].details;
			if(entries[i].details.at(entries[i].details.size()-1) != '\n') {
				stream << "\n";
			}
		}

		if(!entries[i].fileMessage.empty()) {
			stream << "[" << (i+1) << "] File      : " << entries[i].fileMessage;
			if(entries[i].fileMessage.at(entries[i].fileMessage.size()-1) != '\n') {
				stream << "\n";
			}
		}

		if(entries[i].stacktrace.empty()) {
			stream << "[" << (i+1) << "] Stacktrace: not available\n";
		}
		else {
			stream << "[" << (i+1) << "] Stacktrace: " << entries[i].stacktrace << "\n";
		}

		if(i+1 < entries.size()) {
			stream << "Nested exception:\n";
		}
	}
}

void ExceptionHandler::dump(esl::logging::StreamReal& stream, esl::logging::Location location) const {
	for(std::size_t i=0; i<entries.size(); ++i) {
		stream(location.object, location.function, location.file, location.line) << "[" << (i+1) << "] Exception : " << entries[i].exceptionType << "\n";
		stream(location.object, location.function, location.file, location.line) << "[" << (i+1) << "] What      : " << entries[i].what << "\n";

		if(!entries[i].details.empty()) {
			stream(location.object, location.function, location.file, location.line) << "[" << (i+1) << "] Details   : " << entries[i].details;
			if(entries[i].details.at(entries[i].details.size()-1) != '\n') {
				stream(location.object, location.function, location.file, location.line) << "\n";
			}
		}

		if(!entries[i].fileMessage.empty()) {
			stream(location.object, location.function, location.file, location.line) << "[" << (i+1) << "] File     : " << entries[i].fileMessage;
			if(entries[i].fileMessage.at(entries[i].fileMessage.size()-1) != '\n') {
				stream(location.object, location.function, location.file, location.line) << "\n";
			}
		}

		if(entries[i].stacktrace.empty()) {
			stream(location.object, location.function, location.file, location.line) << "[" << (i+1) << "] Stacktrace: not available\n";
		}
		else {
			stream(location.object, location.function, location.file, location.line) << "[" << (i+1) << "] Stacktrace:" << entries[i].stacktrace << "\n";
		}

		if(i+1 < entries.size()) {
			stream(location.object, location.function, location.file, location.line) << "Nested exception:\n";
		}
	}
}

void ExceptionHandler::initialize(std::exception_ptr exceptionPointer) {
	if(!exceptionPointer) {
		return;
	}

	try {
		std::rethrow_exception(exceptionPointer);
	}
	catch(const esl::com::http::server::exception::StatusCode& e) {
		std::string details;
		std::string what;
		std::string exceptionType;

		details = "HTTP status code " + std::to_string(e.getStatusCode());
		if(e.what() && std::string(e.what()) != esl::com::http::server::exception::StatusCode::getMessage(e.getStatusCode())) {
			what = e.what();
		}
		else {
			what = HttpStatusCode::getMessage(e.getStatusCode());
		}

		addEntry(e, "esl::com::http::server::exception::StatusCode", what.c_str(), details);

		try {
			std::rethrow_if_nested(e);
		}
		catch (...) {
			initialize(std::current_exception());
		}
	}
	catch(const esl::database::exception::SqlError& e) {
		std::stringstream details;

		details << "SQL Return Code:" << e.getSqlReturnCode() << "\n";
		const esl::database::Diagnostics& diagnostics = e.getDiagnostics();
		diagnostics.dump(details);

		addEntry(e, "esl::database::exception::SqlError", e.what(), details.str());

		try {
			std::rethrow_if_nested(e);
		}
		catch (...) {
			initialize(std::current_exception());
		}
	}
	catch(const esl::plugin::exception::PluginNotFound& e) {
		std::string details;

		const esl::plugin::Registry::BasePlugins& basePlugins = esl::plugin::Registry::get().getPlugins(e.getTypeIndex());
		if(basePlugins.empty()) {
			details = "No implementations available.\n";
		}
		else {
			details = "Implementations available:\n";
			for(const auto& basePlugin : basePlugins) {
				details += "- " + basePlugin.first + "\n";
			}
		}

		addEntry(e, "esl::plugin::exception::PluginNotFound", e.what(), details);

		try {
			std::rethrow_if_nested(e);
		}
		catch (...) {
			initialize(std::current_exception());
		}
	}
	catch(const std::runtime_error& e) {
		addEntry(e, "std::runtime_error", e.what());

		try {
			std::rethrow_if_nested(e);
		}
		catch (...) {
			initialize(std::current_exception());
		}
	}
	catch(std::out_of_range& e) {
		addEntry(e, "std::out_of_range", e.what());

		try {
			std::rethrow_if_nested(e);
		}
		catch (...) {
			initialize(std::current_exception());
		}
	}
	catch(std::invalid_argument& e) {
		addEntry(e, "std::invalid_argument", e.what());

		try {
			std::rethrow_if_nested(e);
		}
		catch (...) {
			initialize(std::current_exception());
		}
	}
	catch(std::logic_error& e) {
		addEntry(e, "std::logic_error", e.what());

		try {
			std::rethrow_if_nested(e);
		}
		catch (...) {
			initialize(std::current_exception());
		}
	}
	catch(const std::exception& e) {
		addEntry(e, "std::exception", e.what());

		try {
			std::rethrow_if_nested(e);
		}
		catch (...) {
			initialize(std::current_exception());
		}
	}
	catch (...) {
		Entry entry;
		entry.exceptionType = "unknown exception";
		entries.push_back(std::move(entry));
	}
}

void ExceptionHandler::addEntry(const std::exception& e, const std::string& exceptionType, const char* what, const std::string& details) {
	Entry entry;

	entry.exceptionType = exceptionType;
	entry.what = what;
	entry.details = details;
	if(showStacktrace) {
		entry.stacktrace = createStackstrace(esl::system::Stacktrace::get(e));
	}
	if(showFilePosition) {
		entry.fileMessage = createFileMessage(esl::io::FilePosition::get(e));
	}

	entries.push_back(std::move(entry));
}

} /* namespace common4esl */
