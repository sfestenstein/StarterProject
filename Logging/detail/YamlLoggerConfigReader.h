#ifndef LOGGING_YAMLLOGGERCONFIGREADER_H_
#define LOGGING_YAMLLOGGERCONFIGREADER_H_

#include "Logging/detail/LoggerConfig.h"

#include <string>

namespace CommonUtils
{
namespace Logging
{

bool LoadConfigFile(const std::string &configPath, LoggerConfig *config, std::string *errorMessage);

}
}

#endif