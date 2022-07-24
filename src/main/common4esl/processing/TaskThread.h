#ifndef COMMON4ESL_PROCESSING_TASKTHREAD_H_
#define COMMON4ESL_PROCESSING_TASKTHREAD_H_

#include <mutex>
#include <thread>

namespace common4esl {
namespace processing {

class TaskFactory;

class TaskThread {
public:
	static void create(TaskFactory& taskFactory);

private:
	TaskFactory& taskFactory;

	TaskThread(TaskFactory& taskFactory);
	~TaskThread();

	static void run(TaskFactory& taskFactory);
};

} /* namespace processing */
} /* namespace common4esl */

#endif /* COMMON4ESL_PROCESSING_TASKTHREAD_H_ */
