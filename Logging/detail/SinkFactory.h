#ifndef LOGGING_SINKFACTORY_H_
#define LOGGING_SINKFACTORY_H_

#include "Logging/Logger.h"
#include "Logging/detail/LoggerConfig.h"

#include <map>
#include <memory>

#include "spdlog/logger.h"

namespace CommonUtils
{
namespace Logging
{

std::shared_ptr<spdlog::logger> BuildFallbackLogger(const LoggerConfig &config);
std::shared_ptr<spdlog::logger> BuildLogger(
    const LoggerConfig &config,
    const std::map<std::string, CustomSinkFactory> &customFactories);

}
}

#endif