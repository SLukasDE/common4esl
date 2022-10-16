#ifndef COMMON4ESL_LOGGER_H_
#define COMMON4ESL_LOGGER_H_

#include <esl/logging/Logger.h>
#include <esl/logging/Level.h>

namespace common4esl {

#ifdef COMMON4ESL_LOGGING_LEVEL_DEBUG
using Logger = esl::logging::Logger<esl::logging::Level::TRACE>;
#else
using Logger = esl::logging::Logger<esl::logging::Level::WARN>;
#endif

} /* namespace common4esl */

#endif /* COMMON4ESL_LOGGER_H_ */
