#ifndef COMMON4ESL_CONFIG_CONTEXT_ENTRY_H_
#define COMMON4ESL_CONFIG_CONTEXT_ENTRY_H_

#include <common4esl/config/Config.h>
#include <common4esl/processing/Context.h>

#include <string>
#include <ostream>

namespace common4esl {
namespace config {
namespace context {

class Entry : public Config {
public:
	using Config::Config;

	virtual void save(std::ostream& oStream, std::size_t spaces) const = 0;
	virtual void install(processing::Context& context) const = 0;
};

} /* namespace context */
} /* namespace config */
} /* namespace common4esl */

#endif /* COMMON4ESL_CONFIG_CONTEXT_ENTRY_H_ */
