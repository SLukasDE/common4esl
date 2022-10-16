#include <common4esl/processing/TaskBinding.h>
#include <common4esl/processing/TaskFactory.h>

#include <common4esl/object/Context.h>

namespace common4esl {
namespace processing {

TaskBinding::TaskBinding(TaskFactory& aTaskFactory, esl::processing::TaskDescriptor aDescriptor)
: taskFactory(&aTaskFactory),
  descriptor(std::move(aDescriptor)),
  event(dynamic_cast<esl::object::Event*>(descriptor.procedure.get()))
{ }

void TaskBinding::sendEvent(const esl::object::Object& object) {
	if(event) {
		event->onEvent(object);
	}
}

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
				setStatus(esl::processing::Status::canceled);
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

esl::processing::Status TaskBinding::getStatus() const {
	return status.load();
}

esl::object::Context* TaskBinding::getContext() const {
	switch(getStatus()) {
	case esl::processing::Status::canceled:
	case esl::processing::Status::exception:
	case esl::processing::Status::done:
		return descriptor.context.get();
	default:
		break;
	}
	return nullptr;
}

std::exception_ptr TaskBinding::getException() const {
	return exceptionPtr;
}

void TaskBinding::setStatus(esl::processing::Status aStatus) {
	if(status.exchange(aStatus) == aStatus) {
		return;
	}

	if(aStatus == esl::processing::Status::canceled) {
		std::lock_guard<std::mutex> lockTaskFactory(taskFactoryMutex);
		taskFactory = nullptr;
	}

	if(descriptor.onStateChanged) {
		descriptor.onStateChanged(aStatus);
	}
}

void TaskBinding::run() noexcept {
	try {
		setStatus(esl::processing::Status::running);
		if(!descriptor.context) {
			descriptor.context.reset(new common4esl::object::Context);
		}
		if(descriptor.procedure) {
			descriptor.procedure->procedureRun(*descriptor.context);
		}
		setStatus(esl::processing::Status::done);
	}
	catch(...) {
		exceptionPtr = std::current_exception();
		try {
			setStatus(esl::processing::Status::exception);
		} catch(...) { }
	}

	std::lock_guard<std::mutex> lockTaskFactory(taskFactoryMutex);
	taskFactory = nullptr;
}

} /* namespace processing */
} /* namespace common4esl */
