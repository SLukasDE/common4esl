#include <esl/object/SimpleContext.h>

#include <common4esl/object/Context.h>

#include <stdexcept>

namespace esl {
inline namespace v1_6 {
namespace object {

SimpleContext::Settings::Settings(const std::vector<std::pair<std::string, std::string>>& settings) {
	for(const auto& setting : settings) {
		throw std::runtime_error("Unknown key \"" + setting.first + "\"");
    }
}

SimpleContext::SimpleContext()
: context(new common4esl::object::Context(Settings()))
{ }

SimpleContext::SimpleContext(const Settings& settings)
: context(new common4esl::object::Context(Settings(settings)))
{ }

std::unique_ptr<Context> SimpleContext::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<Context>(new SimpleContext(Settings(settings)));
}

void SimpleContext::addObject(const std::string& id, std::unique_ptr<Object> object) {
	context->addObject(id, std::move(object));
}

std::set<std::string> SimpleContext::getObjectIds() const {
	return context->getObjectIds();
}

Object* SimpleContext::findRawObject(const std::string& id) {
	return context->findObject<Object>(id);
}

const Object* SimpleContext::findRawObject(const std::string& id) const {
	return context->findObject<Object>(id);
}

} /* namespace object */
} /* inline namespace v1_6 */
} /* namespace esl */
