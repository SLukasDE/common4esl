#ifndef COMMON4ESL_CONFIG_CONTEXT_ENTRY_H_
#define COMMON4ESL_CONFIG_CONTEXT_ENTRY_H_

#include <common4esl/config/Config.h>
#include <common4esl/object/ProcessingContext.h>

#include <string>
#include <ostream>

namespace common4esl {
inline namespace v1_6 {
namespace config {
namespace context {

class Entry : public Config {
public:
	using Config::Config;

	virtual void save(std::ostream& oStream, std::size_t spaces) const = 0;
	virtual void install(object::ProcessingContext& context) const = 0;
};

} /* namespace context */
} /* namespace config */
} /* inline namespace v1_6 */
} /* namespace common4esl */

#endif /* COMMON4ESL_CONFIG_CONTEXT_ENTRY_H_ */
