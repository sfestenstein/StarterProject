#include <gtest/gtest.h>

#include <stdexcept>

#include "Logging/detail/LogUtils.h"

using CommonUtils::Logging::ToSpdlogLevel;
using CommonUtils::Logging::kLevelCritical;
using CommonUtils::Logging::kLevelDebug;
using CommonUtils::Logging::kLevelError;
using CommonUtils::Logging::kLevelInfo;
using CommonUtils::Logging::kLevelOff;
using CommonUtils::Logging::kLevelTrace;
using CommonUtils::Logging::kLevelWarn;
using CommonUtils::Logging::kLevelWarning;

TEST(LogUtilsTest, MapsToSpdlogLevels)
{
    EXPECT_EQ(ToSpdlogLevel(kLevelTrace), spdlog::level::trace);
    EXPECT_EQ(ToSpdlogLevel(kLevelDebug), spdlog::level::debug);
    EXPECT_EQ(ToSpdlogLevel(kLevelInfo), spdlog::level::info);
    EXPECT_EQ(ToSpdlogLevel(kLevelWarn), spdlog::level::warn);
    EXPECT_EQ(ToSpdlogLevel(kLevelWarning), spdlog::level::warn);
    EXPECT_EQ(ToSpdlogLevel(kLevelError), spdlog::level::err);
    EXPECT_EQ(ToSpdlogLevel(kLevelCritical), spdlog::level::critical);
    EXPECT_EQ(ToSpdlogLevel(kLevelOff), spdlog::level::off);
}

TEST(LogUtilsTest, IsCaseInsensitive)
{
    EXPECT_EQ(ToSpdlogLevel("DEBUG"), spdlog::level::debug);
    EXPECT_EQ(ToSpdlogLevel("InFo"), spdlog::level::info);
}

TEST(LogUtilsTest, ThrowsOnUnknownLevel)
{
    EXPECT_THROW(ToSpdlogLevel("banana"), std::runtime_error);
}