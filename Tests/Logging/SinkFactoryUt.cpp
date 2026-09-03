#include <gtest/gtest.h>

#include <atomic>
#include <map>

#include "Logging/detail/SinkFactory.h"
#include "spdlog/sinks/sink.h"

using CommonUtils::Logging::BuildFallbackLogger;
using CommonUtils::Logging::BuildLogger;
using CommonUtils::Logging::CustomSinkFactory;
using CommonUtils::Logging::ICustomSink;
using CommonUtils::Logging::LoggerConfig;
using CommonUtils::Logging::SinkConfig;
using CommonUtils::Logging::SinkType;

namespace
{

class CountingSink : public ICustomSink
{
public:
    explicit CountingSink(std::atomic<int> *count) : count_(count) {}

    void LogDebug(const std::string &) override
    {
        count_->fetch_add(1);
    }

    void LogInfo(const std::string &) override
    {
        count_->fetch_add(1);
    }

    void LogError(const std::string &) override
    {
        count_->fetch_add(1);
    }

    void LogCrit(const std::string &) override
    {
        count_->fetch_add(1);
    }

private:
    std::atomic<int> *count_;
};

} // namespace

TEST(SinkFactoryTest, BuildsFallbackLogger)
{
    LoggerConfig config;
    config.pattern = "%v";

    std::shared_ptr<spdlog::logger> logger = BuildFallbackLogger(config);

    ASSERT_TRUE(logger != nullptr);
    EXPECT_EQ(logger->name(), "fallback");
    EXPECT_EQ(logger->level(), spdlog::level::trace);
    EXPECT_EQ(logger->sinks().size(), 1u);
}

TEST(SinkFactoryTest, BuildsConfiguredLoggerWithSinkLevels)
{
    LoggerConfig config;
    config.level = "debug";
    config.pattern = "%v";

    SinkConfig console;
    console.type = SinkType::Console;
    console.level = "warn";
    config.sinks.push_back(console);

    SinkConfig rotating;
    rotating.type = SinkType::RotatingFile;
    rotating.level = "error";
    rotating.filePath = "/tmp/sink_factory_test.log";
    rotating.maxFileSizeBytes = 1024;
    rotating.maxFiles = 2;
    config.sinks.push_back(rotating);

    std::map<std::string, CustomSinkFactory> customFactories;
    std::shared_ptr<spdlog::logger> logger = BuildLogger(config, customFactories);

    ASSERT_TRUE(logger != nullptr);
    EXPECT_EQ(logger->name(), "general");
    EXPECT_EQ(logger->level(), spdlog::level::debug);
    ASSERT_EQ(logger->sinks().size(), 2u);
    EXPECT_EQ(logger->sinks()[0]->level(), spdlog::level::warn);
    EXPECT_EQ(logger->sinks()[1]->level(), spdlog::level::err);
}

TEST(SinkFactoryTest, ReturnsNullWhenOnlyUnregisteredCustomSink)
{
    LoggerConfig config;
    config.level = "info";

    SinkConfig custom;
    custom.type = SinkType::Custom;
    custom.name = "otel";
    custom.level = "info";
    config.sinks.push_back(custom);

    std::map<std::string, CustomSinkFactory> customFactories;
    std::shared_ptr<spdlog::logger> logger = BuildLogger(config, customFactories);

    EXPECT_TRUE(logger == nullptr);
}

TEST(SinkFactoryTest, UsesRegisteredCustomSink)
{
    std::atomic<int> messageCount(0);

    LoggerConfig config;
    config.level = "info";
    config.pattern = "%v";

    SinkConfig custom;
    custom.type = SinkType::Custom;
    custom.name = "counting";
    custom.level = "info";
    config.sinks.push_back(custom);

    std::map<std::string, CustomSinkFactory> customFactories;
    customFactories["counting"] = [&messageCount](const SinkConfig &) {
        return std::shared_ptr<ICustomSink>(new CountingSink(&messageCount));
    };

    std::shared_ptr<spdlog::logger> logger = BuildLogger(config, customFactories);

    ASSERT_TRUE(logger != nullptr);
    EXPECT_EQ(logger->name(), "general");

    logger->log(spdlog::level::info, "hello custom sink");
    EXPECT_EQ(messageCount.load(), 1);
}