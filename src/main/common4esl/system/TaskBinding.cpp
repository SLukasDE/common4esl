#include <common4esl/object/Context.h>
#include <common4esl/system/TaskBinding.h>
#include <common4esl/system/TaskFactory.h>

#include <esl/object/Procedure.h>

namespace common4esl {
inline namespace v1_6 {
namespace system {

TaskBinding::TaskBinding(TaskFactory& aTaskFactory, esl::system::Task::Descriptor aDescriptor)
: taskFactory(&aTaskFactory),
  descriptor(std::move(aDescriptor))
{ }

void TaskBinding::cancel() {
	std::lock_guard<std::mutex> lockTaskFactory(taskFactoryMutex);

	if(taskFactory == nullptr) {
		return;
	}

	{
		std::lock_guard<std::mutex> lockQueueMutext(taskFactory->queueMutex);
		for(auto iter = taskFactory->queue.begin(); iter != taskFactory->queue.end(); ++iter) {
			if(iter->first == this) {
				taskFactory->queue.erase(iter);
				setStatus(esl::system::Task::Status::canceled);
				return;
			}
		}
	}

	if(descriptor.procedure) {
		std::lock_guard<std::mutex> lockThreadsMutext(taskFactory->threadsMutex);
		if(taskFactory->threadsProcessing.count(this) != 0) {
			descriptor.procedure->procedureCancel();
		}
	}
}

esl::system::Task::Status TaskBinding::getStatus() const {
	return status.load();
}

esl::object::Context* TaskBinding::getContext() const {
	switch(getStatus()) {
	case esl::system::Task::Status::canceled:
	case esl::system::Task::Status::exception:
	case esl::system::Task::Status::done:
		return descriptor.context.get();
	default:
		break;
	}
	return nullptr;
}

std::exception_ptr TaskBinding::getException() const {
	return exceptionPtr;
}

void TaskBinding::setStatus(esl::system::Task::Status aStatus) {
	if(status.exchange(aStatus) == aStatus) {
		return;
	}

	if(aStatus == esl::system::Task::Status::canceled) {
		std::lock_guard<std::mutex> lockTaskFactory(taskFactoryMutex);
		taskFactory = nullptr;
	}

	if(descriptor.onStateChanged) {
		descriptor.onStateChanged(aStatus);
	}
}

void TaskBinding::run() noexcept {
	try {
		setStatus(esl::system::Task::Status::running);
		if(!descriptor.context) {
			descriptor.context.reset(new object::Context);
		}
		if(descriptor.procedure) {
			descriptor.procedure->procedureRun(*descriptor.context);
		}
		setStatus(esl::system::Task::Status::done);
	}
	catch(...) {
		exceptionPtr = std::current_exception();
		try {
			setStatus(esl::system::Task::Status::exception);
		} catch(...) { }
	}

	std::lock_guard<std::mutex> lockTaskFactory(taskFactoryMutex);
	taskFactory = nullptr;
}

} /* namespace system */
} /* inline namespace v1_6 */
} /* namespace common4esl */
