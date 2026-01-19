
#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <functional>
#include <sstream>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <memory>

#include <thread>

// Platform-specific syslog support
#if defined(__linux__) || defined(__APPLE__)
    #include <syslog.h>
    #define LOGGER_HAS_SYSLOG 1
#else
    #define LOGGER_HAS_SYSLOG 0
#endif

#define LOGGER_INFO(...) LOGGER_INFO_IMPL(__VA_ARGS__)
#define LOGGER_WARN(...) LOGGER_WARN_IMPL(__VA_ARGS__)
#define LOGGER_ERROR(...) LOGGER_ERROR_IMPL(__VA_ARGS__)
#define LOGGER_DEBUG(...) LOGGER_DEBUG_IMPL(__VA_ARGS__)
#define LOGGER_TRACE(...) LOGGER_TRACE_IMPL(__VA_ARGS__)

// Implementation macros that handle both cases
#define LOGGER_INFO_IMPL(...) LOGGER_GET_MACRO(__VA_ARGS__, LOGGER_INFO_NAMED, LOGGER_INFO_DEFAULT)(__VA_ARGS__)
#define LOGGER_WARN_IMPL(...) LOGGER_GET_MACRO(__VA_ARGS__, LOGGER_WARN_NAMED, LOGGER_WARN_DEFAULT)(__VA_ARGS__)
#define LOGGER_ERROR_IMPL(...) LOGGER_GET_MACRO(__VA_ARGS__, LOGGER_ERROR_NAMED, LOGGER_ERROR_DEFAULT)(__VA_ARGS__)
#define LOGGER_DEBUG_IMPL(...) LOGGER_GET_MACRO(__VA_ARGS__, LOGGER_DEBUG_NAMED, LOGGER_DEBUG_DEFAULT)(__VA_ARGS__)
#define LOGGER_TRACE_IMPL(...) LOGGER_GET_MACRO(__VA_ARGS__, LOGGER_TRACE_NAMED, LOGGER_TRACE_DEFAULT)(__VA_ARGS__)

// Helper macro to select between named and default versions
#define LOGGER_GET_MACRO(_1, _2, NAME, ...) NAME

// Default logger versions (1 argument)
#define LOGGER_INFO_DEFAULT(msg) core::Logger::getInstance().log("INFO", msg, __FILE__, __LINE__, __func__)
#define LOGGER_WARN_DEFAULT(msg) core::Logger::getInstance().log("WARN", msg, __FILE__, __LINE__, __func__)
#define LOGGER_ERROR_DEFAULT(msg) core::Logger::getInstance().log("ERROR", msg, __FILE__, __LINE__, __func__)
#define LOGGER_DEBUG_DEFAULT(msg) core::Logger::getInstance().log("DEBUG", msg, __FILE__, __LINE__, __func__)
#define LOGGER_TRACE_DEFAULT(msg) core::Logger::getInstance().log("TRACE", msg, __FILE__, __LINE__, __func__)

// Named logger versions (2 arguments)
#define LOGGER_INFO_NAMED(name, msg) core::Logger::getInstance(name).log("INFO", msg, __FILE__, __LINE__, __func__)
#define LOGGER_WARN_NAMED(name, msg) core::Logger::getInstance(name).log("WARN", msg, __FILE__, __LINE__, __func__)
#define LOGGER_ERROR_NAMED(name, msg) core::Logger::getInstance(name).log("ERROR", msg, __FILE__, __LINE__, __func__)
#define LOGGER_DEBUG_NAMED(name, msg) core::Logger::getInstance(name).log("DEBUG", msg, __FILE__, __LINE__, __func__)
#define LOGGER_TRACE_NAMED(name, msg) core::Logger::getInstance(name).log("TRACE", msg, __FILE__, __LINE__, __func__)

// Enhanced macros with format support and early level check
#define LOGGER_INFO_F(...) LOGGER_INFO_F_IMPL(__VA_ARGS__)
#define LOGGER_WARN_F(...) LOGGER_WARN_F_IMPL(__VA_ARGS__)
#define LOGGER_ERROR_F(...) LOGGER_ERROR_F_IMPL(__VA_ARGS__)
#define LOGGER_DEBUG_F(...) LOGGER_DEBUG_F_IMPL(__VA_ARGS__)
#define LOGGER_TRACE_F(...) LOGGER_TRACE_F_IMPL(__VA_ARGS__)

// Implementation helpers for format macros
#define LOGGER_INFO_F_IMPL(...) LOGGER_GET_F_MACRO(__VA_ARGS__, LOGGER_INFO_F_NAMED, LOGGER_INFO_F_DEFAULT, DUMMY)(__VA_ARGS__)
#define LOGGER_WARN_F_IMPL(...) LOGGER_GET_F_MACRO(__VA_ARGS__, LOGGER_WARN_F_NAMED, LOGGER_WARN_F_DEFAULT, DUMMY)(__VA_ARGS__)
#define LOGGER_ERROR_F_IMPL(...) LOGGER_GET_F_MACRO(__VA_ARGS__, LOGGER_ERROR_F_NAMED, LOGGER_ERROR_F_DEFAULT, DUMMY)(__VA_ARGS__)
#define LOGGER_DEBUG_F_IMPL(...) LOGGER_GET_F_MACRO(__VA_ARGS__, LOGGER_DEBUG_F_NAMED, LOGGER_DEBUG_F_DEFAULT, DUMMY)(__VA_ARGS__)
#define LOGGER_TRACE_F_IMPL(...) LOGGER_GET_F_MACRO(__VA_ARGS__, LOGGER_TRACE_F_NAMED, LOGGER_TRACE_F_DEFAULT, DUMMY)(__VA_ARGS__)

// Helper macro for format versions (needs at least 2 args)
#define LOGGER_GET_F_MACRO(_1, _2, _3, NAME, ...) NAME

// Default format versions
#define LOGGER_INFO_F_DEFAULT(fmt, ...)                                                           \
    do                                                                                            \
    {                                                                                             \
        if (core::Logger::getInstance().getCurrentLevel() <= core::Logger::Level::INFO)           \
        {                                                                                         \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                   \
            core::Logger::getInstance().log("INFO", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                         \
    } while (0)

#define LOGGER_WARN_F_DEFAULT(fmt, ...)                                                           \
    do                                                                                            \
    {                                                                                             \
        if (core::Logger::getInstance().getCurrentLevel() <= core::Logger::Level::WARN)           \
        {                                                                                         \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                   \
            core::Logger::getInstance().log("WARN", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                         \
    } while (0)

#define LOGGER_ERROR_F_DEFAULT(fmt, ...)                                                           \
    do                                                                                             \
    {                                                                                              \
        if (core::Logger::getInstance().getCurrentLevel() <= core::Logger::Level::ERROR)           \
        {                                                                                          \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                    \
            core::Logger::getInstance().log("ERROR", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                          \
    } while (0)

#define LOGGER_DEBUG_F_DEFAULT(fmt, ...)                                                           \
    do                                                                                             \
    {                                                                                              \
        if (core::Logger::getInstance().getCurrentLevel() <= core::Logger::Level::DEBUG)           \
        {                                                                                          \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                    \
            core::Logger::getInstance().log("DEBUG", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                          \
    } while (0)

#define LOGGER_TRACE_F_DEFAULT(fmt, ...)                                                           \
    do                                                                                             \
    {                                                                                              \
        if (core::Logger::getInstance().getCurrentLevel() <= core::Logger::Level::TRACE)           \
        {                                                                                          \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                    \
            core::Logger::getInstance().log("TRACE", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                          \
    } while (0)

// Named format versions
#define LOGGER_INFO_F_NAMED(name, fmt, ...)                                                           \
    do                                                                                                \
    {                                                                                                 \
        if (core::Logger::getInstance(name).getCurrentLevel() <= core::Logger::Level::INFO)           \
        {                                                                                             \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                       \
            core::Logger::getInstance(name).log("INFO", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                             \
    } while (0)

#define LOGGER_WARN_F_NAMED(name, fmt, ...)                                                           \
    do                                                                                                \
    {                                                                                                 \
        if (core::Logger::getInstance(name).getCurrentLevel() <= core::Logger::Level::WARN)           \
        {                                                                                             \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                       \
            core::Logger::getInstance(name).log("WARN", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                             \
    } while (0)

#define LOGGER_ERROR_F_NAMED(name, fmt, ...)                                                           \
    do                                                                                                 \
    {                                                                                                  \
        if (core::Logger::getInstance(name).getCurrentLevel() <= core::Logger::Level::ERROR)           \
        {                                                                                              \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                        \
            core::Logger::getInstance(name).log("ERROR", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                              \
    } while (0)

#define LOGGER_DEBUG_F_NAMED(name, fmt, ...)                                                           \
    do                                                                                                 \
    {                                                                                                  \
        if (core::Logger::getInstance(name).getCurrentLevel() <= core::Logger::Level::DEBUG)           \
        {                                                                                              \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                        \
            core::Logger::getInstance(name).log("DEBUG", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                              \
    } while (0)

#define LOGGER_TRACE_F_NAMED(name, fmt, ...)                                                           \
    do                                                                                                 \
    {                                                                                                  \
        if (core::Logger::getInstance(name).getCurrentLevel() <= core::Logger::Level::TRACE)           \
        {                                                                                              \
            std::string formatted_msg = core::detail::format(fmt, __VA_ARGS__);                        \
            core::Logger::getInstance(name).log("TRACE", formatted_msg, __FILE__, __LINE__, __func__); \
        }                                                                                              \
    } while (0)

// Conditional logging macros
#define LOGGER_INFO_IF(condition, msg) \
    do                                 \
    {                                  \
        if (condition)                 \
            LOGGER_INFO(msg);          \
    } while (0)

#define LOGGER_ERROR_IF(condition, msg) \
    do                                  \
    {                                   \
        if (condition)                  \
            LOGGER_ERROR(msg);          \
    } while (0)

namespace core
{
    // Optimized formatting utility for logger
    namespace detail
    {
        // Pre-scan format string to find all placeholder positions (done once per format string)
        inline std::vector<size_t> find_placeholders(const std::string &fmt)
        {
            std::vector<size_t> positions;
            size_t pos = 0;
            while ((pos = fmt.find("{}", pos)) != std::string::npos)
            {
                positions.push_back(pos);
                pos += 2;
            }
            return positions;
        }

        // Optimized non-recursive implementation
        template <typename... Args>
        std::string format(const std::string &fmt, Args &&...args)
        {
            if constexpr (sizeof...(args) == 0)
            {
                return fmt;
            }
            else
            {
                // Pre-allocate string with reasonable capacity
                std::string result;
                result.reserve(fmt.size() + sizeof...(args) * 16); // Estimate extra space needed

                // Find all placeholder positions
                static thread_local std::vector<size_t> positions;
                positions.clear();

                size_t pos = 0;
                while ((pos = fmt.find("{}", pos)) != std::string::npos)
                {
                    positions.push_back(pos);
                    pos += 2;
                    if (positions.size() >= sizeof...(args))
                        break; // Don't search beyond needed
                }

                if (positions.empty())
                {
                    return fmt; // No placeholders found
                }

                // Build result string efficiently
                size_t last_pos = 0;
                size_t arg_index = 0;

                auto append_arg = [&result, &arg_index](auto &&arg)
                {
                    if (arg_index < positions.size())
                    {
                        if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg)>>)
                        {
                            // Optimize for numeric types
                            result += std::to_string(arg);
                        }
                        else
                        {
                            // Use stringstream for other types
                            std::ostringstream oss;
                            oss << arg;
                            result += oss.str();
                        }
                    }
                    ++arg_index;
                };

                // Process each placeholder
                size_t placeholder_index = 0;
                ((placeholder_index < positions.size() ? (result += fmt.substr(last_pos, positions[placeholder_index] - last_pos),
                                                          append_arg(std::forward<Args>(args)),
                                                          last_pos = positions[placeholder_index] + 2,
                                                          ++placeholder_index, void())
                                                       : void()),
                 ...);

                // Append remaining format string
                if (last_pos < fmt.size())
                {
                    result += fmt.substr(last_pos);
                }

                return result;
            }
        }

        // Specialized versions for common cases to avoid template overhead
        inline std::string format(const std::string &fmt)
        {
            return fmt;
        }

        template <typename T>
        std::string format(const std::string &fmt, T &&arg)
        {
            size_t pos = fmt.find("{}");
            if (pos == std::string::npos)
                return fmt;

            std::string result;
            result.reserve(fmt.size() + 16);
            result += fmt.substr(0, pos);

            if constexpr (std::is_arithmetic_v<std::decay_t<T>>)
            {
                result += std::to_string(arg);
            }
            else
            {
                std::ostringstream oss;
                oss << arg;
                result += oss.str();
            }
            result += fmt.substr(pos + 2);
            return result;
        }
    }

    class Logger
    {
    public:
        // Constants for buffer sizes and limits
        static constexpr size_t FORMATTED_LOG_BUFFER_SIZE = 1024;
        static constexpr size_t DEFAULT_MESSAGE_RESERVE_SIZE = 256;
        static constexpr size_t DEFAULT_MAX_MESSAGE_LENGTH = 4096;

        enum class Level
        {
            TRACE = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            NONE
        };

        static Logger &getInstance()
        {
            static Logger instance;
            return instance;
        }

        static Logger &getInstance(const std::string &name)
        {
            static std::unordered_map<std::string, std::unique_ptr<Logger>> namedLoggers;
            static std::mutex namedLoggersMutex;

            std::lock_guard<std::mutex> lock(namedLoggersMutex);
            auto it = namedLoggers.find(name);
            if (it == namedLoggers.end())
            {
                namedLoggers[name] = std::unique_ptr<Logger>(new Logger(name));
            }
            return *namedLoggers[name];
        }
        void setLogFile(const std::string &filename)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (logFile_.is_open())
                logFile_.close();

            logFilePath_ = filename; // Store the file path
            logFile_.open(filename, std::ios::out | std::ios::app);
            if (!logFile_.is_open())
            {
                std::cerr << "[LOGGER ERROR] Failed to open log file: " << filename
                          << ", falling back to stderr" << std::endl;
#if defined(_WIN32)
                std::cerr << "[LOGGER ERROR] Reason: " << std::strerror(errno) << std::endl;
#else
                std::cerr << "[LOGGER ERROR] Reason: " << std::strerror(errno) << std::endl;
#endif
                logFileFailed_ = true;
                logFilePath_.clear(); // Clear path on failure
            }
            else
            {
                logFileFailed_ = false;
                // Disable colors for file output - files should be plain text
                fileColorOutput_ = false;
            }
        }

        void clearLog(const std::string &name = "")
        {
            if (name.empty())
            {
                // Clear default logger
                clearLogFile();
            }
            else
            {
                // Clear named logger
                auto &logger = getInstance(name);
                logger.clearLogFile();
            }
        }

        void clearLogFile()
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (logFile_.is_open() && !logFilePath_.empty())
            {
                logFile_.close();
                logFile_.open(logFilePath_, std::ios::out | std::ios::trunc);
                if (logFile_.is_open())
                {
                    logFileFailed_ = false;
                    // Disable colors for file output - files should be plain text
                    fileColorOutput_ = false;
                }
                else
                {
                    logFileFailed_ = true;
                }
            }
        }

        void setLevel(Level level)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            minLevel_ = level;
        }

        // Main log function with source location
        void log(const std::string &levelStr, const std::string &msg,
                 const char *file = nullptr, int line = 0, const char *func = nullptr)
        {
            Level level = parseLevel(levelStr);
            if (level < minLevel_)
                return;

            std::lock_guard<std::mutex> lock(mutex_);
            std::string timestamp = getTimestamp();
            std::string threadId = getThreadId();

#if LOGGER_HAS_SYSLOG
            if (useSyslog_)
            {
                int syslogLevel = LOG_INFO;
                if (level == Level::ERROR)
                    syslogLevel = LOG_ERR;
                else if (level == Level::WARN)
                    syslogLevel = LOG_WARNING;
                else if (level == Level::DEBUG)
                    syslogLevel = LOG_DEBUG;
                else if (level == Level::TRACE)
                    syslogLevel = LOG_DEBUG; // Use DEBUG for TRACE in syslog

                std::string formatted = formatLog(levelStr, msg, timestamp, threadId, file, line, func, false);
                syslog(syslogLevel, "%s", formatted.c_str());
            }
            else
#endif
            if (logFile_.is_open() && !logFileFailed_)
            {
                // Format for file without colors
                std::string formatted = formatLog(levelStr, msg, timestamp, threadId, file, line, func, fileColorOutput_);
                logFile_ << formatted << std::endl;
                // Check for write errors
                if (logFile_.fail())
                {
                    logFileFailed_ = true;
                    std::cerr << "[LOGGER ERROR] Failed to write to log file, falling back to stderr" << std::endl;
                    // Format for console with colors
                    std::string consoleFormatted = formatLog(levelStr, msg, timestamp, threadId, file, line, func, colorOutput_);
                    std::cerr << consoleFormatted << std::endl;
                }
            }
            else
            {
                // Format for console with colors
                std::string formatted = formatLog(levelStr, msg, timestamp, threadId, file, line, func, colorOutput_);
                std::cerr << formatted << std::endl;
            }
        }

    public:
        /**
         * @brief Enable syslog output (Linux/macOS only).
         * @note On Windows, this function has no effect.
         */
        void enableSyslog(bool enable = true)
        {
#if LOGGER_HAS_SYSLOG
            std::lock_guard<std::mutex> lock(mutex_);
            useSyslog_ = enable;
#else
            (void)enable; // Suppress unused parameter warning
#endif
        }

        // Enhanced configuration methods
        void setColorOutput(bool enable = true)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            colorOutput_ = enable;
        }

        void setMaxMessageLength(size_t maxLen)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            maxMessageLength_ = maxLen;
        }

        // Get current configuration state
        Level getCurrentLevel() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return minLevel_;
        }

        bool isFileLoggingEnabled() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return logFile_.is_open() && !logFileFailed_;
        }

        const std::string &getName() const
        {
            return name_;
        }

        ~Logger()
        {
            if (logFile_.is_open())
                logFile_.close();
        }

    private:
        // Buffer size constants
        static constexpr size_t TIMESTAMP_BUFFER_SIZE = 32;
        static constexpr size_t TIMESTAMP_RESULT_SIZE = 48;
        static constexpr size_t THREAD_ID_BUFFER_SIZE = 32;
        static constexpr unsigned int THREAD_ID_HASH_MASK = 0xFFFFFFFF;
        static constexpr size_t LOG_LEVEL_WIDTH = 5; // Width for log level alignment

        Logger() : name_("default"), minLevel_(Level::TRACE), useSyslog_(false), showThreadId_(false),
                   showFileLineFunc_(false), logFileFailed_(false), colorOutput_(true), fileColorOutput_(true),
                   maxMessageLength_(DEFAULT_MAX_MESSAGE_LENGTH)
        {
            initializeFromEnvironment();
        }

        Logger(const std::string &name) : name_(name), minLevel_(Level::TRACE), useSyslog_(false), showThreadId_(false),
                                          showFileLineFunc_(false), logFileFailed_(false), colorOutput_(true), fileColorOutput_(true),
                                          maxMessageLength_(DEFAULT_MAX_MESSAGE_LENGTH)
        {
            initializeFromEnvironment();
        }
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        std::string name_;
        Level minLevel_;
        std::ofstream logFile_;
        std::string logFilePath_; // Store the log file path for clearing
        mutable std::mutex mutex_;
        bool useSyslog_;
        bool showThreadId_;
        bool showFileLineFunc_;
        bool logFileFailed_;
        bool colorOutput_;     // Color output for console
        bool fileColorOutput_; // Color output for file (should be false)
        size_t maxMessageLength_;

    public:
        void showThreadId(bool show = true)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            showThreadId_ = show;
        }

        void showFileLineFunc(bool show = true)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            showFileLineFunc_ = show;
        }

    private:
        Level parseLevel(const std::string &levelStr) const
        {
            if (levelStr == "TRACE")
                return Level::TRACE;
            if (levelStr == "DEBUG")
                return Level::DEBUG;
            if (levelStr == "INFO")
                return Level::INFO;
            if (levelStr == "WARN")
                return Level::WARN;
            if (levelStr == "ERROR")
                return Level::ERROR;
            return Level::NONE;
        }

        const char *getColor(const std::string &level) const
        {
            if (!colorOutput_)
                return "";

            if (level == "INFO")
                return "\033[32m"; // Green
            if (level == "WARN")
                return "\033[33m"; // Yellow
            if (level == "ERROR")
                return "\033[31m"; // Red
            if (level == "DEBUG")
                return "\033[36m"; // Cyan
            if (level == "TRACE")
                return "\033[35m"; // Magenta
            return "\033[0m";      // Reset/default
        }

        std::string getTimestamp() const
        {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            // Use char buffer for better performance
            char buffer[TIMESTAMP_BUFFER_SIZE];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&in_time_t));

            // Append milliseconds efficiently - increased buffer size to prevent truncation
            char result[TIMESTAMP_RESULT_SIZE];
            std::snprintf(result, sizeof(result), "%s.%03d", buffer, static_cast<int>(ms.count()));
            return std::string(result);
        }

        std::string getThreadId() const
        {
            // More efficient thread ID formatting
            std::hash<std::thread::id> hasher;
            auto hash = hasher(std::this_thread::get_id());
            char buffer[THREAD_ID_BUFFER_SIZE];
            std::snprintf(buffer, sizeof(buffer), "%08x", static_cast<unsigned int>(hash & THREAD_ID_HASH_MASK));
            return std::string(buffer);
        }

        std::string formatLog(const std::string &level, const std::string &msg,
                              const std::string &timestamp, const std::string &threadId,
                              const char *file, int line, const char *func, bool useColors) const
        {
            // Pre-calculate approximate size to reduce reallocations
            std::string result;
            result.reserve(DEFAULT_MESSAGE_RESERVE_SIZE); // Most log messages are under 256 chars

            // Bold timestamp (only if color is enabled)
            if (useColors)
                result += "\033[1m";
            result += "[";
            result += timestamp;
            result += "]";
            if (useColors)
                result += "\033[0m";
            result += " ";

            // Thread ID if enabled
            if (showThreadId_)
            {
                result += "[";
                result += threadId;
                result += "] ";
            }

            // Colorized level with fixed width for alignment
            if (useColors)
                result += getColor(level);
            result += "[";

            // Pad level to fixed width for better alignment
            if (level.length() < LOG_LEVEL_WIDTH)
            {
                result += level;
                result += std::string(LOG_LEVEL_WIDTH - level.length(), ' '); // Right-pad with spaces
            }
            else
            {
                result += level.substr(0, LOG_LEVEL_WIDTH); // Truncate if too long
            }

            result += "]";
            if (useColors)
                result += "\033[0m";
            result += " ";

            // File/line/function if enabled
            if (showFileLineFunc_ && file)
            {
                result += file;
                result += ":";
                result += std::to_string(line);
                result += " (";
                result += func;
                result += ") ";
            }

            // Message (truncate if too long)
            if (msg.length() > maxMessageLength_)
            {
                result += msg.substr(0, maxMessageLength_ - 3);
                result += "...";
            }
            else
            {
                result += msg;
            }

            return result;
        }

        void initializeFromEnvironment()
        {
            // Check for log level environment variable
            const char *logLevel = std::getenv("ADSIL_LOG_LEVEL");
            if (logLevel)
            {
                Level level = parseLevel(std::string(logLevel));
                if (level != Level::NONE)
                    minLevel_ = level;
            }

            // Check for log file environment variable
            const char *logFile = std::getenv("ADSIL_LOG_FILE");
            if (logFile)
            {
                setLogFile(std::string(logFile));
            }

            // Check for color output setting
            const char *colorEnv = std::getenv("ADSIL_LOG_COLOR");
            if (colorEnv)
            {
                std::string colorStr(colorEnv);
                colorOutput_ = (colorStr == "1" || colorStr == "true" || colorStr == "yes");
            }

            // Check for thread ID display
            const char *threadEnv = std::getenv("ADSIL_LOG_THREAD_ID");
            if (threadEnv)
            {
                std::string threadStr(threadEnv);
                showThreadId_ = (threadStr == "1" || threadStr == "true" || threadStr == "yes");
            }

            // Check for file/line/function display
            const char *sourceEnv = std::getenv("ADSIL_LOG_SOURCE_INFO");
            if (sourceEnv)
            {
                std::string sourceStr(sourceEnv);
                showFileLineFunc_ = (sourceStr == "1" || sourceStr == "true" || sourceStr == "yes");
            }
        }
    };
} // namespace core
