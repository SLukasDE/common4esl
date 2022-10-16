#include <common4esl/Logger.h>
#include <common4esl/processing/TaskBinding.h>
#include <common4esl/processing/TaskFactory.h>
#include <common4esl/processing/TaskThread.h>

#include <chrono>
#include <memory>
#include <thread>

namespace common4esl {
namespace processing {
namespace {
Logger logger("common4esl::processing::TaskThread");
}

void TaskThread::create(TaskFactory& taskFactory) {
	if(logger.trace) {
		logger.trace << "threadsMax:               " << taskFactory.threadsMax << "\n";
		logger.trace << "threadsAvailable:         " << taskFactory.threadsAvailable << "\n";
		logger.trace << "threadsProcessing.size(): " << taskFactory.threadsProcessing.size() << "\n";
	}

	++taskFactory.threadsAvailable;
	//std::thread thread(&Thread::run, taskFactory);
	std::thread thread([&taskFactory]() {
		TaskThread t(taskFactory);
	});
	thread.detach();
}

TaskThread::TaskThread(TaskFactory& aTaskFactory)
: taskFactory(aTaskFactory)
{
	logger.trace << "Thread " << std::hash<std::thread::id>{}(std::this_thread::get_id()) << ": created\n";

	while(taskFactory.threadsMax.load() != 0) {
		while([this]() {
			if(taskFactory.threadsMax.load() == 0) {
				return false;
			}

			std::lock_guard<std::mutex> lockQueueMutex(taskFactory.queueMutex);
			return !taskFactory.queue.empty();
		}()) {
			TaskBinding* bindingPtr;
			std::shared_ptr<esl::processing::Task::Binding> binding;

			logger.trace << "Thread " << std::hash<std::thread::id>{}(std::this_thread::get_id()) << ": fetch binding from queue...\n";

			{
				std::lock_guard<std::mutex> lockQueueMutex(taskFactory.queueMutex);
				if(taskFactory.queue.empty()) {
					break;
				}
				bindingPtr = taskFactory.queue.front().first;
				binding = taskFactory.queue.front().second;
				taskFactory.queue.pop_front();
			}

			/* Insert binding to threadsProcessing */
			{
				std::unique_lock<std::mutex> lockThreadsMutex(taskFactory.threadsMutex);
				taskFactory.threadsProcessing.insert(std::make_pair(bindingPtr, std::move(binding)));
			}

			/* Run procedure by calling "run"-wrapper, to manage status, exceptions, ... */
			bindingPtr->run();

			/* Remove binding from threadsProcessing */
			{
				std::unique_lock<std::mutex> lockThreadsMutex(taskFactory.threadsMutex);
				taskFactory.threadsProcessing.erase(bindingPtr);
			}
		}

		logger.trace << "Thread " << std::hash<std::thread::id>{}(std::this_thread::get_id()) << ": wait for new task in queue\n";

		std::unique_lock<std::mutex> lockThreadsMutex(taskFactory.threadsMutex);
		if(taskFactory.threadsCV.wait_for(lockThreadsMutex, taskFactory.threadTimeout, [this]() {
			return taskFactory.threadsMax.load() == 0 ||  !taskFactory.queue.empty();
		}) == false) {
			logger.trace << "Thread " << std::hash<std::thread::id>{}(std::this_thread::get_id()) << ": timeout or cancel -> exit task!\n";
			break;
		}

		logger.trace << "Thread " << std::hash<std::thread::id>{}(std::this_thread::get_id()) << ": new task in queue\n";
	}

	logger.trace << "Thread " << std::hash<std::thread::id>{}(std::this_thread::get_id()) << ": finished loop\n";
}

TaskThread::~TaskThread() {
	{
		std::unique_lock<std::mutex> lockThreadsMutex(taskFactory.threadsMutex);
		--taskFactory.threadsAvailable;
	}

	taskFactory.threadsFinishedCV.notify_one();

}

void TaskThread::run(TaskFactory& taskFactory) {
	TaskThread thread(taskFactory);
}

} /* namespace processing */
} /* namespace common4esl */
