#ifndef COMMON4ESL_SYSTEM_TASKFACTORY_H_
#define COMMON4ESL_SYSTEM_TASKFACTORY_H_

#include <common4esl/system/TaskBinding.h>

#include <esl/object/Object.h>
#include <esl/system/DefaultTaskFactory.h>
#include <esl/system/Task.h>
#include <esl/system/TaskFactory.h>

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
inline namespace v1_6 {
namespace system {

class TaskThread;

class TaskFactory final : public esl::system::TaskFactory {
public:
	friend class TaskBinding;
	friend class TaskThread;

	TaskFactory(const esl::system::DefaultTaskFactory::Settings& settings);
	~TaskFactory();

#ifdef ESL_1_6
	std::shared_ptr<Task> createTask(std::unique_ptr<Procedure> procedure, std::function<void(Status)> onStateChanged, unsigned int priority = 0, std::unique_ptr<object::Context> context) override;
	std::vector<std::shared_ptr<Task>> getTasks() const override;
#else
	esl::system::Task createTask(esl::system::Task::Descriptor descriptor) override;
	std::vector<esl::system::Task> getTasks() const override;
#endif

private:
	mutable std::mutex queueMutex; // mutable because of "getTasks() const"
#ifdef ESL_1_6
#else
	std::list<std::pair<TaskBinding*, std::shared_ptr<esl::system::Task::Binding>>> queue;
#endif

	mutable std::mutex threadsMutex; // mutable because of "getTasks() const"
	std::condition_variable threadsCV;
	std::atomic<unsigned int> threadsMax { 0 };
	unsigned int threadsAvailable = 0;
	std::map<TaskBinding*, std::shared_ptr<esl::system::Task::Binding>> threadsProcessing;
	std::condition_variable threadsFinishedCV;

	const std::chrono::milliseconds threadTimeout { 1000 };
};

} /* namespace system */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_SYSTEM_TASKFACTORY_H_ */
