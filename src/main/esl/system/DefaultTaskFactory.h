#ifndef ESL_SYSTEM_DEFAULTTASKFACTORY_H_
#define ESL_SYSTEM_DEFAULTTASKFACTORY_H_

#include <esl/system/Task.h>
#include <esl/system/TaskFactory.h>

#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace esl {
inline namespace v1_6 {
namespace system {

class DefaultTaskFactory : public TaskFactory {
public:
	struct Settings {
		Settings(const std::vector<std::pair<std::string, std::string>>& settings);

		unsigned int threadsMax { 0 };
		std::chrono::milliseconds threadTimeout { 1000 };
	};

	DefaultTaskFactory(const Settings& settings);

	static std::unique_ptr<TaskFactory> create(const std::vector<std::pair<std::string, std::string>>& settings);

#ifdef ESL_1_6
	std::shared_ptr<Task> createTask(std::unique_ptr<Procedure> procedure, std::function<void(Status)> onStateChanged, unsigned int priority = 0, std::unique_ptr<object::Context> context = nullptr) override;
	std::vector<std::shared_ptr<Task>> getTasks() const override;
#else
	Task createTask(Task::Descriptor descriptor) override;
	std::vector<Task> getTasks() const override;
#endif

private:
	std::unique_ptr<TaskFactory> taskFactory;
};

} /* namespace system */
} /* inline namespace v1_6 */
} /* namespace esl */

#endif /* ESL_SYSTEM_DEFAULTTASKFACTORY_H_ */
