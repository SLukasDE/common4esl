#include <esl/system/DefaultTaskFactory.h>
//#include <esl/system/Stacktrace.h>

#include <common4esl/system/TaskFactory.h>

#ifdef __linux__
#include <sys/sysinfo.h>
#endif

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace system {

DefaultTaskFactory::Settings::Settings(const std::vector<std::pair<std::string, std::string>>& settings) {
	bool hasThreadTimeout = false;

    for(const auto& setting : settings) {
		if(setting.first == "max-threads") {
			if(threadsMax > 0) {
		        throw std::runtime_error("multiple definition of attribute 'max-threads'.");
			}

			int tmpMaxThreads;
			try {
				tmpMaxThreads = std::stol(setting.second);
			}
			catch(...) {
	            throw std::runtime_error("Invalid value \"" + setting.second + "\" for attribute 'max-threads'.");
			}

			if(tmpMaxThreads <= 0 || tmpMaxThreads > 1000) {
	            throw std::runtime_error("Invalid value \"" + std::to_string(tmpMaxThreads) + "\" for attribute 'max-threads'. Value has to be between 1 and 1000.");
			}
			threadsMax = static_cast<unsigned int>(tmpMaxThreads);
		}
		else if(setting.first == "thread-timeout-ms") {
			if(hasThreadTimeout) {
		        throw std::runtime_error("multiple definition of attribute 'thread-timeout-ms'.");
			}
			hasThreadTimeout = true;
			//long threadTimeoutMs = static_cast<int>(std::stol(setting.second));
			long threadTimeoutMs = std::stol(setting.second);
			if(threadTimeoutMs < 1) {
		    	throw std::runtime_error("Invalid value \"" + setting.second + "\" for key 'thread-timeout-ms'. Value must be > 0");
			}
			threadTimeout = std::chrono::milliseconds(threadTimeoutMs);
		}
		else {
            throw std::runtime_error("unknown attribute '\"" + setting.first + "\"'.");
		}
    }

#ifdef __linux__
	if(threadsMax == 0) {
	    int nProcs = get_nprocs();
		if(nProcs > 0) {
			threadsMax = static_cast<unsigned int>(nProcs);
		}
	}
#endif

	if(threadsMax == 0) {
		throw std::runtime_error("Definition of 'max-threads' is missing.");
	}
};

DefaultTaskFactory::DefaultTaskFactory(const Settings& settings)
: taskFactory(new common4esl::system::TaskFactory(std::move(settings)))
{ }

std::unique_ptr<TaskFactory> DefaultTaskFactory::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<TaskFactory>(new DefaultTaskFactory(Settings(settings)));
}

#ifdef ESL_1_6

std::shared_ptr<Task> DefaultTaskFactory::createTask(std::unique_ptr<Procedure> procedure, std::function<void(Status)> onStateChanged, unsigned int priority, std::unique_ptr<object::Context> context = nullptr) {
	return taskFactory->createTask(std::move(procedure), onStateChanged, priority, std::move(context));
}

std::vector<std::shared_ptr<Task>> DefaultTaskFactory::getTasks() const {
	return taskFactory->getTasks();
}

#else
Task DefaultTaskFactory::createTask(Task::Descriptor descriptor) {
	return taskFactory->createTask(std::move(descriptor));
}

std::vector<Task> DefaultTaskFactory::getTasks() const {
	return taskFactory->getTasks();
}
#endif

} /* namespace system */
} /* inline namespace v1_6 */
} /* namespace esl */
