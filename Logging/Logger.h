#ifndef LOGGING_LOGGER_H_
#define LOGGING_LOGGER_H_

#include "Logging/SinkConfig.h"

#include <memory>
#include <functional>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace CommonUtils
{
namespace Logging
{

class ICustomSink
{
public:
    virtual ~ICustomSink() {}
    virtual void LogDebug(const std::string &message) = 0;
    virtual void LogInfo(const std::string &message) = 0;
    virtual void LogError(const std::string &message) = 0;
    virtual void LogCrit(const std::string &message) = 0;
};

typedef std::function<std::shared_ptr<ICustomSink>(const SinkConfig &config)> CustomSinkFactory;

namespace detail
{

inline void AppendRemaining(std::ostringstream &out)
{
    (void)out;
}

template <typename T, typename... Args>
void AppendRemaining(std::ostringstream &out, T &&value, Args &&... args)
{
    out << ' ' << std::forward<T>(value);
    AppendRemaining(out, std::forward<Args>(args)...);
}

inline void FormatIntoStream(std::ostringstream &out, const char *format)
{
    out << format;
}

template <typename T, typename... Args>
void FormatIntoStream(std::ostringstream &out, const char *format, T &&value, Args &&... args)
{
    const char *placeholder = std::strstr(format, "{}");

    if (placeholder == nullptr)
    {
        out << format;
        AppendRemaining(out, std::forward<T>(value), std::forward<Args>(args)...);
        return;
    }

    out.write(format, static_cast<std::streamsize>(placeholder - format));
    out << std::forward<T>(value);
    FormatIntoStream(out, placeholder + 2, std::forward<Args>(args)...);
}

template <typename... Args>
std::string FormatMessage(const char *format, Args &&... args)
{
    std::ostringstream out;
    FormatIntoStream(out, format, std::forward<Args>(args)...);
    return out.str();
}

} // namespace detail

void RegisterCustomSinkFactory(const std::string &sinkName, CustomSinkFactory factory);
bool InitFromFile(const std::string &configPath, std::string *errorMessage = nullptr);
void Flush();
void LogDebug(const std::string &message);
void LogInfo(const std::string &message);
void LogError(const std::string &message);
void LogCrit(const std::string &message);
bool IsInitialized();

} // namespace Logging
} // namespace CommonUtils

#endif