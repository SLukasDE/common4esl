#ifndef COMMON4ESL_PROCESSING_TASKBINDING_H_
#define COMMON4ESL_PROCESSING_TASKBINDING_H_

#include <esl/object/Context.h>
#include <esl/object/Event.h>
#include <esl/processing/Procedure.h>
#include <esl/processing/TaskDescriptor.h>
#include <esl/processing/TaskFactory.h>
#include <esl/processing/Status.h>
#include <esl/processing/Task.h>

#include <atomic>
#include <chrono>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>

namespace common4esl {
namespace processing {

class TaskFactory;
class TaskThread;

class TaskBinding final : public esl::processing::Task::Binding {
public:
	friend class Thread;
	friend class Task;

	TaskBinding(TaskFactory& taskFactory, esl::processing::TaskDescriptor descriptor);

	void sendEvent(const esl::object::Object& object) override;
	void cancel() override;

	esl::processing::Status getStatus() const override;
	esl::object::Context* getContext() const override;
	std::exception_ptr getException() const override;

	void setStatus(esl::processing::Status status);

	/* called by Thread::run() */
	void run() noexcept;

private:
	mutable std::mutex taskFactoryMutex;
	TaskFactory* taskFactory;

	esl::processing::TaskDescriptor descriptor;
	esl::object::Event* event = nullptr;

	std::atomic<esl::processing::Status> status { esl::processing::Status::waiting };
	std::exception_ptr exceptionPtr;
};

} /* namespace processing */
} /* namespace common4esl */

#endif /* COMMON4ESL_PROCESSING_TASKBINDING_H_ */
