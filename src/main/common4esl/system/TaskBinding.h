#ifndef COMMON4ESL_SYSTEM_TASKBINDING_H_
#define COMMON4ESL_SYSTEM_TASKBINDING_H_

#include <esl/object/Context.h>
//#include <esl/object/Event.h>
#include <esl/system/Task.h>
#include <esl/system/TaskFactory.h>

#include <atomic>
#include <chrono>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>

namespace common4esl {
inline namespace v1_6 {
namespace system {

class TaskFactory;
class TaskThread;

class TaskBinding final : public esl::system::Task::Binding {
public:
	friend class Thread;
	friend class Task;

	TaskBinding(TaskFactory& taskFactory, esl::system::Task::Descriptor descriptor);

	void cancel() override;

	esl::system::Task::Status getStatus() const override;
	esl::object::Context* getContext() const override;
	std::exception_ptr getException() const override;

	void setStatus(esl::system::Task::Status status);

	/* called by Thread::run() */
	void run() noexcept;

private:
	mutable std::mutex taskFactoryMutex;
	TaskFactory* taskFactory;

	esl::system::Task::Descriptor descriptor;

	std::atomic<esl::system::Task::Status> status { esl::system::Task::Status::waiting };
	std::exception_ptr exceptionPtr;
};

} /* namespace system */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_SYSTEM_TASKBINDING_H_ */
