#include "Logging/Logger.h"

#include "Logging/detail/LogUtils.h"
#include "Logging/detail/SinkFactory.h"
#include "Logging/detail/YamlLoggerConfigReader.h"

#include <map>
#include <memory>
#include <mutex>
#include <spdlog/logger.h>

namespace CommonUtils
{
namespace Logging
{
namespace
{

LoggerConfig MakeFallbackConfig();

struct LoggerState
{
    LoggerState()
        : config(MakeFallbackConfig()),
          configured(false)
    {
        std::shared_ptr<spdlog::logger> fallbackLogger = BuildFallbackLogger(config);
        std::atomic_store(&logger, fallbackLogger);
    }

    std::mutex mutex;
    std::shared_ptr<spdlog::logger> logger;
    LoggerConfig config;
    bool configured = false;
    std::map<std::string, CustomSinkFactory> customFactories;
};
LoggerState globalState;

LoggerConfig MakeFallbackConfig()
{
    LoggerConfig config;
    config.level = kLevelInfo;

    SinkConfig sink;
    sink.type = SinkType::Console;
    sink.level = kLevelTrace;
    sink.name = "fallback_console";
    config.sinks.push_back(sink);

    return config;
}

} // namespace

void ApplyConfig(const LoggerConfig &config);
void ResetToFallback();

void ResetToFallback()
{
    LoggerConfig fallbackConfig = MakeFallbackConfig();
    std::shared_ptr<spdlog::logger> fallbackLogger = BuildFallbackLogger(fallbackConfig);

    std::lock_guard<std::mutex> lock(globalState.mutex);
    globalState.config = fallbackConfig;
    std::atomic_store(&globalState.logger, fallbackLogger);
    globalState.configured = false;
}

void RegisterCustomSinkFactory(const std::string &sinkName, CustomSinkFactory factory)
{
    std::lock_guard<std::mutex> lock(globalState.mutex);
    globalState.customFactories[sinkName] = factory;
}

bool InitFromFile(const std::string &configPath, std::string *errorMessage)
{
    LoggerConfig config;
    if (!LoadConfigFile(configPath, &config, errorMessage))
    {
        ResetToFallback();
        return false;
    }

    ApplyConfig(config);
    return true;
}

void ApplyConfig(const LoggerConfig &config)
{
    std::map<std::string, CustomSinkFactory> customFactories;
    {
        std::lock_guard<std::mutex> lock(globalState.mutex);
        customFactories = globalState.customFactories;
    }

    std::shared_ptr<spdlog::logger> logger = BuildLogger(config, customFactories);
    if (!logger)
    {
        std::shared_ptr<spdlog::logger> fallbackLogger = BuildFallbackLogger(config);
        if (fallbackLogger)
        {
            fallbackLogger->error("No valid sinks were configured. Logging is now disabled.");
            fallbackLogger->flush();
        }
    }

    {
        std::lock_guard<std::mutex> lock(globalState.mutex);
        globalState.config = config;
        std::atomic_store(&globalState.logger, logger);
        globalState.configured = true;
    }
}

void Flush()
{
    std::shared_ptr<spdlog::logger> logger = std::atomic_load(&globalState.logger);
    if (logger)
    {
        logger->flush();
    }
}

void LogDebug(const std::string &message)
{
    std::shared_ptr<spdlog::logger> logger = std::atomic_load(&globalState.logger);
    if (logger)
    {
        logger->debug(message);
    }
}

void LogInfo(const std::string &message)
{
    std::shared_ptr<spdlog::logger> logger = std::atomic_load(&globalState.logger);
    if (logger)
    {
        logger->info(message);
    }
}

void LogError(const std::string &message)
{
    std::shared_ptr<spdlog::logger> logger = std::atomic_load(&globalState.logger);
    if (logger)
    {
        logger->error(message);
    }
}

void LogCrit(const std::string &message)
{
    std::shared_ptr<spdlog::logger> logger = std::atomic_load(&globalState.logger);
    if (logger)
    {
        logger->critical(message);
    }
}

bool IsInitialized()
{
    std::lock_guard<std::mutex> lock(globalState.mutex);
    return globalState.configured;
}

} // namespace Logging
} // namespace CommonUtils