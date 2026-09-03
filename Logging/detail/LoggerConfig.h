#ifndef LOGGING_DETAIL_LOGGERCONFIG_H_
#define LOGGING_DETAIL_LOGGERCONFIG_H_

#include "Logging/SinkConfig.h"

#include <string>
#include <vector>

namespace CommonUtils
{
namespace Logging
{

struct LoggerConfig
{
    std::string level = "info";
    std::string pattern = "%H%M%S.%e [%t] %v";
    std::vector<SinkConfig> sinks;
};

}
}

#endif