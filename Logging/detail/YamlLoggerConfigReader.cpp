#include "Logging/detail/YamlLoggerConfigReader.h"

#include "Logging/detail/LogUtils.h"

#include "yaml-cpp/yaml.h"

#include <stdexcept>

namespace CommonUtils
{
namespace Logging
{
namespace
{

SinkType ParseSinkType(const std::string &typeText)
{
    if (typeText == "console")
        return SinkType::Console;
    if (typeText == "rotating_file")
        return SinkType::RotatingFile;
    if (typeText == "custom")
        return SinkType::Custom;

    throw std::runtime_error("unknown sink type: " + typeText);
}

} // namespace

bool LoadConfigFile(const std::string &configPath, LoggerConfig *config, std::string *errorMessage)
{
    if (config == nullptr)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = "config output pointer was null";
        }
        return false;
    }

    try
    {
        YAML::Node root = YAML::LoadFile(configPath);
        LoggerConfig parsed;

        if (root["level"])
        {
            parsed.level = root["level"].as<std::string>();
            (void)ToSpdlogLevel(parsed.level);
        }

        if (root["pattern"])
        {
            parsed.pattern = root["pattern"].as<std::string>();
        }

        if (root["sinks"])
        {
            for (const auto &sinkNode : root["sinks"])
            {
                SinkConfig sink;
                sink.yamlNode = YAML::Load(YAML::Dump(sinkNode));
                sink.type = sinkNode["type"] ? ParseSinkType(sinkNode["type"].as<std::string>()) : SinkType::Console;
                sink.level = sinkNode["level"] ? sinkNode["level"].as<std::string>() : sink.level;
                (void)ToSpdlogLevel(sink.level);
                sink.colored = sinkNode["colored"] ? sinkNode["colored"].as<bool>() : sink.colored;

                if (sinkNode["name"])
                {
                    sink.name = sinkNode["name"].as<std::string>();
                }

                if (sinkNode["path"])
                {
                    sink.filePath = sinkNode["path"].as<std::string>();
                }

                if (sinkNode["max_file_size_bytes"])
                {
                    sink.maxFileSizeBytes = sinkNode["max_file_size_bytes"].as<std::size_t>();
                }

                if (sinkNode["max_files"])
                {
                    sink.maxFiles = sinkNode["max_files"].as<std::size_t>();
                }

                parsed.sinks.push_back(sink);
            }
        }

        *config = parsed;
        if (errorMessage != nullptr)
        {
            errorMessage->clear();
        }
        return true;
    }
    catch (const std::exception &ex)
    {
        if (errorMessage != nullptr)
        {
            *errorMessage = ex.what();
        }
        return false;
    }
}

} // namespace Logging
} // namespace CommonUtils