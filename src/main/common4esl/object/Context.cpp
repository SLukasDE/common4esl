#include <common4esl/object/Context.h>

#include <esl/system/Stacktrace.h>

#include <stdexcept>
#include <utility>

namespace common4esl {
namespace object {

std::set<std::string> Context::getObjectIds() const {
	std::set<std::string> rv;
	for(const auto& object : objects) {
		rv.insert(object.first);
	}
	return rv;
}

void Context::addRawObject(const std::string& id, std::unique_ptr<esl::object::Object> object) {
	if(objects.insert(std::make_pair(id, std::move(object))).second == false) {
		throw esl::system::Stacktrace::add(std::runtime_error("Cannot add element \"" + id + "\" to context because there exists already an object with same id"));
	}
}

esl::object::Object* Context::findRawObject(const std::string& id) {
	auto iter = objects.find(id);
	return iter == std::end(objects) ? nullptr : iter->second.get();
}

const esl::object::Object* Context::findRawObject(const std::string& id) const {
	auto iter = objects.find(id);
	return iter == std::end(objects) ? nullptr : iter->second.get();
}

} /* namespace object */
} /* namespace common4esl */
