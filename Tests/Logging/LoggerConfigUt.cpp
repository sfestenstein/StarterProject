#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <atomic>

#include "yaml-cpp/yaml.h"

#include "Logging/Logger.h"
#include "Logging/LoggerMacros.h"
#include "Logging/detail/LoggerConfig.h"
#include "Logging/detail/YamlLoggerConfigReader.h"

using CommonUtils::Logging::InitFromFile;
using CommonUtils::Logging::IsInitialized;
using CommonUtils::Logging::RegisterCustomSinkFactory;
using CommonUtils::Logging::ICustomSink;
using CommonUtils::Logging::LoadConfigFile;
using CommonUtils::Logging::LoggerConfig;

namespace
{

const char *kConfigPath = "/tmp/starterproject_logging_test.yaml";

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

void WriteTestConfig()
{
    std::ofstream out(kConfigPath);
    out << "level: debug\n";
    out << "pattern: \"%v\"\n";
    out << "sinks:\n";
    out << "  - type: console\n";
    out << "    level: debug\n";
    out << "  - type: rotating_file\n";
    out << "    level: info\n";
    out << "    path: \"/tmp/starterproject_logging_test.log\"\n";
    out << "    max_file_size_bytes: 1024\n";
    out << "    max_files: 2\n";
    out << "  - type: custom\n";
    out << "    name: otel\n";
    out << "    endpoint: \"http://localhost:4317\"\n";
    out << "    service_name: \"starter-project\"\n";
}

} // namespace

TEST(LoggingConfigTest, LoadsYamlConfig)
{
    WriteTestConfig();

    std::atomic<int> messageCount(0);

    RegisterCustomSinkFactory("otel", [&messageCount](const CommonUtils::Logging::SinkConfig &config)
    {
        EXPECT_EQ(config.yamlNode["endpoint"].as<std::string>(), "http://localhost:4317");
        EXPECT_EQ(config.yamlNode["service_name"].as<std::string>(), "starter-project");
        return std::shared_ptr<ICustomSink>(new CountingSink(&messageCount));
    });

    std::string error;
    ASSERT_TRUE(InitFromFile(kConfigPath, &error)) << error;

    GPINFO("custom sink message");

    EXPECT_EQ(messageCount.load(), 1);
    EXPECT_TRUE(IsInitialized());

    std::ifstream rotatingFile("/tmp/starterproject_logging_test.log");
    EXPECT_TRUE(rotatingFile.good());

    std::remove(kConfigPath);
}

TEST(LoggingConfigTest, InitFromMissingFileFallsBack)
{
    std::remove(kConfigPath);

    std::string error;
    EXPECT_FALSE(InitFromFile(kConfigPath, &error));
    EXPECT_FALSE(error.empty());
    EXPECT_FALSE(IsInitialized());
}

TEST(LoggingConfigTest, CustomSinkFactoryReceivesMessages)
{
    std::atomic<int> messageCount(0);

    RegisterCustomSinkFactory("otel", [&messageCount](const CommonUtils::Logging::SinkConfig &config)
    {
        EXPECT_EQ(config.yamlNode["endpoint"].as<std::string>(), "http://localhost:4317");
        EXPECT_EQ(config.yamlNode["service_name"].as<std::string>(), "starter-project");
        return std::shared_ptr<ICustomSink>(new CountingSink(&messageCount));
    });

    WriteTestConfig();

    std::string error;
    ASSERT_TRUE(InitFromFile(kConfigPath, &error)) << error;
    GPINFO("custom sink message");

    EXPECT_EQ(messageCount.load(), 1);
    EXPECT_TRUE(IsInitialized());

    std::remove(kConfigPath);
}

TEST(LoggingConfigTest, ParsesConsoleSinkColorFromConfig)
{
    {
        std::ofstream out(kConfigPath);
        out << "level: info\n";
        out << "sinks:\n";
        out << "  - type: console\n";
        out << "    level: info\n";
        out << "    colored: true\n";
    }

    LoggerConfig config;
    std::string error;
    ASSERT_TRUE(LoadConfigFile(kConfigPath, &config, &error)) << error;
    ASSERT_EQ(config.sinks.size(), 1u);
    EXPECT_TRUE(config.sinks[0].colored);

    {
        std::ofstream out(kConfigPath);
        out << "level: info\n";
        out << "sinks:\n";
        out << "  - type: console\n";
        out << "    level: info\n";
    }

    ASSERT_TRUE(LoadConfigFile(kConfigPath, &config, &error)) << error;
    ASSERT_EQ(config.sinks.size(), 1u);
    EXPECT_FALSE(config.sinks[0].colored);

    std::remove(kConfigPath);
}