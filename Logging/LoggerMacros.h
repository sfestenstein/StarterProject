#ifndef LOGGING_LOGGERMACROS_H_
#define LOGGING_LOGGERMACROS_H_

#include "Logging/Logger.h"

#define GPCRIT(...) CommonUtils::Logging::LogCrit(CommonUtils::Logging::detail::FormatMessage(__VA_ARGS__))
#define GPERROR(...) CommonUtils::Logging::LogError(CommonUtils::Logging::detail::FormatMessage(__VA_ARGS__))
#define GPINFO(...) CommonUtils::Logging::LogInfo(CommonUtils::Logging::detail::FormatMessage(__VA_ARGS__))
#define GPDEBUG(...) CommonUtils::Logging::LogDebug(CommonUtils::Logging::detail::FormatMessage(__VA_ARGS__))

#endif