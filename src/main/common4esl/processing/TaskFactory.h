#ifndef COMMON4ESL_PROCESSING_TASKFACTORY_H_
#define COMMON4ESL_PROCESSING_TASKFACTORY_H_

#include <common4esl/processing/TaskBinding.h>
//#include <common4esl/processing/TaskThread.h>

#include <esl/object/Object.h>
#include <esl/processing/TaskDescriptor.h>
#include <esl/processing/TaskFactory.h>
#include <esl/processing/Status.h>
#include <esl/processing/Task.h>
//#include <esl/version.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace common4esl {
namespace processing {

class TaskThread;

class TaskFactory final : public esl::processing::TaskFactory {
public:
	friend class TaskBinding;
	friend class TaskThread;

#ifndef ESL_1_6
	static std::unique_ptr<esl::processing::TaskFactory> createTaskFactory(const std::vector<std::pair<std::string, std::string>>& settings);
	static std::unique_ptr<esl::object::Object> createObject(const std::vector<std::pair<std::string, std::string>>& settings);
#endif

	TaskFactory(const std::vector<std::pair<std::string, std::string>>& settings);
	~TaskFactory();

	esl::processing::Task createTask(esl::processing::TaskDescriptor descriptor) override;

	std::vector<esl::processing::Task> getTasks() const override;

private:
	mutable std::mutex queueMutex; // mutable because of "getTasks() const"
	std::list<std::pair<TaskBinding*, std::shared_ptr<esl::processing::Task::Binding>>> queue;

	mutable std::mutex threadsMutex; // mutable because of "getTasks() const"
	std::condition_variable threadsCV;
	std::atomic<unsigned int> threadsMax { 0 };
	unsigned int threadsAvailable = 0;
	std::map<TaskBinding*, std::shared_ptr<esl::processing::Task::Binding>> threadsProcessing;
	std::condition_variable threadsFinishedCV;

	bool hasThreadTimeout = false;
	std::chrono::milliseconds threadTimeout { 1000 };
};

} /* namespace processing */
} /* namespace common4esl */

#endif /* COMMON4ESL_PROCESSING_TASKFACTORY_H_ */
