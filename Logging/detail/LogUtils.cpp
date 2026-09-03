#include "Logging/detail/LogUtils.h"

#include <cctype>
#include <stdexcept>

namespace CommonUtils
{
namespace Logging
{

const char kLevelTrace[] = "trace";
const char kLevelDebug[] = "debug";
const char kLevelInfo[] = "info";
const char kLevelWarn[] = "warn";
const char kLevelWarning[] = "warning";
const char kLevelError[] = "error";
const char kLevelCritical[] = "critical";
const char kLevelOff[] = "off";

namespace
{

std::string ToLower(std::string text)
{
    for (std::string::size_type index = 0; index < text.size(); ++index)
    {
        text[index] = static_cast<char>(std::tolower(static_cast<unsigned char>(text[index])));
    }
    return text;
}

} // namespace

spdlog::level::level_enum ToSpdlogLevel(const std::string &levelText)
{
    const std::string lowered = ToLower(levelText);
    if (lowered == kLevelTrace)
    {
        return spdlog::level::trace;
    }
    if (lowered == kLevelDebug)
    {
        return spdlog::level::debug;
    }
    if (lowered == kLevelInfo)
    {
        return spdlog::level::info;
    }
    if (lowered == kLevelWarn || lowered == kLevelWarning)
    {
        return spdlog::level::warn;
    }
    if (lowered == kLevelError)
    {
        return spdlog::level::err;
    }
    if (lowered == kLevelCritical)
    {
        return spdlog::level::critical;
    }
    if (lowered == kLevelOff)
    {
        return spdlog::level::off;
    }

    throw std::runtime_error("unknown log level: " + levelText);
}

}
}