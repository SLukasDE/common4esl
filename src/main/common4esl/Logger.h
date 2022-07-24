#ifndef COMMON4ESL_LOGGER_H_
#define COMMON4ESL_LOGGER_H_

#include <esl/logging/Logger.h>
#include <esl/logging/Level.h>

namespace common4esl {

using Logger = esl::logging::Logger<esl::logging::Level::TRACE>;

} /* namespace common4esl */

#endif /* COMMON4ESL_LOGGER_H_ */
