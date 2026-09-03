#include "Logging/detail/SinkFactory.h"

#include "Logging/detail/LogUtils.h"

#include <iostream>
#include <vector>

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace CommonUtils
{
namespace Logging
{
namespace
{

bool HasColorRangeMarkers(const std::string &pattern)
{
    return pattern.find("%^") != std::string::npos && pattern.find("%$") != std::string::npos;
}

class CustomSinkAdapter : public spdlog::sinks::sink
{
public:
    explicit CustomSinkAdapter(std::shared_ptr<ICustomSink> sink) : sink_(sink) {}

    void log(const spdlog::details::log_msg &msg) override
    {
        if (!sink_)
        {
            return;
        }

        std::string message(msg.payload.data(), msg.payload.size());
        if (msg.level == spdlog::level::critical)
        {
            sink_->LogCrit(message);
            return;
        }

        if (msg.level == spdlog::level::err)
        {
            sink_->LogError(message);
            return;
        }

        if (msg.level == spdlog::level::warn)
        {
            sink_->LogError(message);
            return;
        }

        if (msg.level == spdlog::level::info)
        {
            sink_->LogInfo(message);
            return;
        }

        if (msg.level == spdlog::level::debug || msg.level == spdlog::level::trace)
        {
            sink_->LogDebug(message);
        }
    }

    void flush() override {}

    void set_pattern(const std::string &) override {}

    void set_formatter(std::unique_ptr<spdlog::formatter>) override {}

private:
    std::shared_ptr<ICustomSink> sink_;
};

std::shared_ptr<spdlog::sinks::sink> BuildConsoleSink()
{
    return std::shared_ptr<spdlog::sinks::sink>(new spdlog::sinks::stdout_sink_mt());
}

std::shared_ptr<spdlog::sinks::sink> BuildColoredConsoleSink()
{
    return std::shared_ptr<spdlog::sinks::sink>(new spdlog::sinks::stdout_color_sink_mt());
}

std::shared_ptr<spdlog::sinks::sink> BuildRotatingFileSink(const SinkConfig &sinkConfig)
{
    return std::shared_ptr<spdlog::sinks::sink>(
        new spdlog::sinks::rotating_file_sink_mt(sinkConfig.filePath, sinkConfig.maxFileSizeBytes, sinkConfig.maxFiles));
}

std::shared_ptr<spdlog::sinks::sink> BuildCustomSink(
    const SinkConfig &sinkConfig,
    const std::map<std::string, CustomSinkFactory> &customFactories)
{
    std::map<std::string, CustomSinkFactory>::const_iterator found = customFactories.find(sinkConfig.name);
    if (found == customFactories.end() || !found->second)
    {
        std::cerr << "Skipping unregistered custom sink: " << sinkConfig.name << std::endl;
        return std::shared_ptr<spdlog::sinks::sink>();
    }

    std::shared_ptr<ICustomSink> customSink = found->second(sinkConfig);
    if (!customSink)
    {
        std::cerr << "Custom sink factory returned null: " << sinkConfig.name << std::endl;
        return std::shared_ptr<spdlog::sinks::sink>();
    }

    return std::shared_ptr<spdlog::sinks::sink>(new CustomSinkAdapter(customSink));
}

} // namespace

std::shared_ptr<spdlog::logger> BuildFallbackLogger(const LoggerConfig &config)
{
    std::vector<spdlog::sink_ptr> sinks;
    std::shared_ptr<spdlog::sinks::sink> consoleSink = BuildConsoleSink();
    consoleSink->set_level(spdlog::level::trace);
    sinks.push_back(consoleSink);

    std::shared_ptr<spdlog::logger> logger(new spdlog::logger("fallback", sinks.begin(), sinks.end()));
    logger->set_pattern(config.pattern);
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::err);
    return logger;
}

std::shared_ptr<spdlog::logger> BuildLogger(
    const LoggerConfig &config,
    const std::map<std::string, CustomSinkFactory> &customFactories)
{
    std::vector<spdlog::sink_ptr> sinks;
    bool hasColoredConsoleSink = false;
    for (std::vector<SinkConfig>::const_iterator it = config.sinks.begin(); it != config.sinks.end(); ++it)
    {
        std::shared_ptr<spdlog::sinks::sink> sink;
        if (it->type == SinkType::Console)
        {
            sink = it->colored ? BuildColoredConsoleSink() : BuildConsoleSink();
            hasColoredConsoleSink = hasColoredConsoleSink || it->colored;
        }
        else if (it->type == SinkType::RotatingFile)
        {
            sink = BuildRotatingFileSink(*it);
        }
        else
        {
            sink = BuildCustomSink(*it, customFactories);
        }

        if (!sink)
        {
            continue;
        }

        sink->set_level(ToSpdlogLevel(it->level));
        sinks.push_back(sink);
    }

    if (sinks.empty())
    {
        return std::shared_ptr<spdlog::logger>();
    }

    std::shared_ptr<spdlog::logger> logger(new spdlog::logger("general", sinks.begin(), sinks.end()));
    std::string loggerPattern = config.pattern;
    if (hasColoredConsoleSink && !HasColorRangeMarkers(loggerPattern))
    {
        loggerPattern = std::string("%^") + loggerPattern + "%$";
    }
    logger->set_pattern(loggerPattern);
    logger->set_level(ToSpdlogLevel(config.level));
    logger->flush_on(spdlog::level::err);
    return logger;
}

}
}