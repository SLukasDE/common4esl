#ifndef COMMON4ESL_SYSTEM_TASKTHREAD_H_
#define COMMON4ESL_SYSTEM_TASKTHREAD_H_

#include <mutex>
#include <thread>

namespace common4esl {
inline namespace v1_6 {
namespace system {

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

} /* namespace system */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_SYSTEM_TASKTHREAD_H_ */
