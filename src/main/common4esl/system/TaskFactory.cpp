#include <common4esl/system/TaskFactory.h>
#include <common4esl/system/TaskThread.h>

#include <esl/Logger.h>

namespace common4esl {
inline namespace v1_6 {
namespace system {
namespace {
esl::Logger logger("common4esl::system::TaskFactory");
}

TaskFactory::TaskFactory(const esl::system::DefaultTaskFactory::Settings& settings)
: threadsMax(settings.threadsMax),
  threadTimeout(settings.threadTimeout)
{ }

TaskFactory::~TaskFactory() {
	{
		std::lock_guard<std::mutex> lockQueueMutex(queueMutex);

		threadsMax.store(0);
		for(auto& entry : queue) {
			entry.first->setStatus(esl::system::Task::Status::canceled);
		}
		queue.clear();
	}

	threadsCV.notify_all();

	std::unique_lock<std::mutex> lockThreadsMutex(threadsMutex);
	threadsFinishedCV.wait(lockThreadsMutex, [&]() {
		return threadsAvailable == 0;
	});
}

esl::system::Task TaskFactory::createTask(esl::system::Task::Descriptor descriptor) {
	std::shared_ptr<esl::system::Task::Binding> binding;

	{
		std::lock_guard<std::mutex> lockQueueMutex(queueMutex);

		std::unique_ptr<TaskBinding> bindingTmp(new TaskBinding(*this, std::move(descriptor)));
		TaskBinding* bindingPtr = bindingTmp.get();
		binding = std::shared_ptr<esl::system::Task::Binding>(bindingTmp.release());
		queue.push_back(std::make_pair(bindingPtr, binding));
	}


	{
		std::lock_guard<std::mutex> lockThreadMutex(threadsMutex);

		if(logger.trace) {
			logger.trace << "threadsMax:               " << threadsMax << "\n";
			logger.trace << "threadsAvailable:         " << threadsAvailable << "\n";
			logger.trace << "threadsProcessing.size(): " << threadsProcessing.size() << "\n";
		}

		if(threadsAvailable < threadsMax) {
//		if(threadsProcessing.size() < threadsMax) {
			logger.trace << "-> create new thread\n";
			TaskThread::create(*this);
		}
		else {
			logger.trace << "-> task queued\n";
		}
	}

	threadsCV.notify_one();

	return esl::system::Task(binding);
}

std::vector<esl::system::Task> TaskFactory::getTasks() const {
	std::vector<esl::system::Task> tasks;

	std::lock_guard<std::mutex> lockQueueMutex(queueMutex);
	for(auto& entry : queue) {
		tasks.push_back(esl::system::Task(entry.second));
	}

	std::lock_guard<std::mutex> lockThreadMutex(threadsMutex);
	for(auto& entry : threadsProcessing) {
		tasks.push_back(esl::system::Task(entry.second));
	}

	return tasks;
}

} /* namespace system */
} /* inline namespace v1_6 */
} /* namespace common4esl */
