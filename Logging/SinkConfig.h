#ifndef LOGGING_SINKCONFIG_H_
#define LOGGING_SINKCONFIG_H_

#include <cstddef>
#include <string>

#include "yaml-cpp/yaml.h"

namespace CommonUtils
{
namespace Logging
{

enum class SinkType
{
    Console,
    RotatingFile,
    Custom
};

struct SinkConfig
{
    SinkType type = SinkType::Console;
    std::string level = "info";
    bool colored = false;
    std::string name;
    std::string filePath;
    std::size_t maxFileSizeBytes = 1024 * 1024 * 5;
    std::size_t maxFiles = 3;
    YAML::Node yamlNode;
};

}
}

#endif