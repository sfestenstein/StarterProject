#ifndef LOGGING_LOGUTILS_H_
#define LOGGING_LOGUTILS_H_

#include <string>

#include "spdlog/common.h"

namespace CommonUtils
{
namespace Logging
{

extern const char kLevelTrace[];
extern const char kLevelDebug[];
extern const char kLevelInfo[];
extern const char kLevelWarn[];
extern const char kLevelWarning[];
extern const char kLevelError[];
extern const char kLevelCritical[];
extern const char kLevelOff[];

spdlog::level::level_enum ToSpdlogLevel(const std::string &levelText);

}
}

#endif