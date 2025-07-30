
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

#include <thread>
#include <syslog.h>

#define LOGGER_INFO(msg) core::Logger::getInstance().log("INFO", msg, __FILE__, __LINE__, __func__)
#define LOGGER_WARN(msg) core::Logger::getInstance().log("WARN", msg, __FILE__, __LINE__, __func__)
#define LOGGER_ERROR(msg) core::Logger::getInstance().log("ERROR", msg, __FILE__, __LINE__, __func__)
#define LOGGER_DEBUG(msg) core::Logger::getInstance().log("DEBUG", msg, __FILE__, __LINE__, __func__)
#define LOGGER_TRACE(msg) core::Logger::getInstance().log("TRACE", msg, __FILE__, __LINE__, __func__)

// Enhanced macros with format support
#define LOGGER_INFO_F(fmt, ...)                                                                     \
    do                                                                                              \
    {                                                                                               \
        char buffer[core::Logger::FORMATTED_LOG_BUFFER_SIZE];                                       \
        std::snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__);                                    \
        core::Logger::getInstance().log("INFO", std::string(buffer), __FILE__, __LINE__, __func__); \
    } while (0)

#define LOGGER_WARN_F(fmt, ...)                                                                     \
    do                                                                                              \
    {                                                                                               \
        char buffer[core::Logger::FORMATTED_LOG_BUFFER_SIZE];                                       \
        std::snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__);                                    \
        core::Logger::getInstance().log("WARN", std::string(buffer), __FILE__, __LINE__, __func__); \
    } while (0)

#define LOGGER_ERROR_F(fmt, ...)                                                                     \
    do                                                                                               \
    {                                                                                                \
        char buffer[core::Logger::FORMATTED_LOG_BUFFER_SIZE];                                        \
        std::snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__);                                     \
        core::Logger::getInstance().log("ERROR", std::string(buffer), __FILE__, __LINE__, __func__); \
    } while (0)

#define LOGGER_DEBUG_F(fmt, ...)                                                                     \
    do                                                                                               \
    {                                                                                                \
        char buffer[core::Logger::FORMATTED_LOG_BUFFER_SIZE];                                        \
        std::snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__);                                     \
        core::Logger::getInstance().log("DEBUG", std::string(buffer), __FILE__, __LINE__, __func__); \
    } while (0)

#define LOGGER_TRACE_F(fmt, ...)                                                                     \
    do                                                                                               \
    {                                                                                                \
        char buffer[core::Logger::FORMATTED_LOG_BUFFER_SIZE];                                        \
        std::snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__);                                     \
        core::Logger::getInstance().log("TRACE", std::string(buffer), __FILE__, __LINE__, __func__); \
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

        void setLogFile(const std::string &filename)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (logFile_.is_open())
                logFile_.close();

            logFile_.open(filename, std::ios::out | std::ios::app);
            if (!logFile_.is_open())
            {
                // Fallback to stderr if file cannot be opened
                std::cerr << "[LOGGER ERROR] Failed to open log file: " << filename
                          << ", falling back to stderr" << std::endl;
                logFileFailed_ = true;
            }
            else
            {
                logFileFailed_ = false;
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
            std::string formatted = formatLog(levelStr, msg, timestamp, threadId, file, line, func);

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
                syslog(syslogLevel, "%s", formatted.c_str());
            }
            else if (logFile_.is_open() && !logFileFailed_)
            {
                logFile_ << formatted << std::endl;
                // Check for write errors
                if (logFile_.fail())
                {
                    logFileFailed_ = true;
                    std::cerr << "[LOGGER ERROR] Failed to write to log file, falling back to stderr" << std::endl;
                    std::cerr << formatted << std::endl;
                }
            }
            else
            {
                std::cerr << formatted << std::endl;
            }
        }

    public:
        void enableSyslog(bool enable = true)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            useSyslog_ = enable;
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

    private:
        // Buffer size constants
        static constexpr size_t TIMESTAMP_BUFFER_SIZE = 32;
        static constexpr size_t TIMESTAMP_RESULT_SIZE = 48;
        static constexpr size_t THREAD_ID_BUFFER_SIZE = 32;
        static constexpr unsigned int THREAD_ID_HASH_MASK = 0xFFFFFFFF;
        static constexpr size_t LOG_LEVEL_WIDTH = 5; // Width for log level alignment

        Logger() : minLevel_(Level::TRACE), useSyslog_(false), showThreadId_(false),
                   showFileLineFunc_(false), logFileFailed_(false), colorOutput_(true),
                   maxMessageLength_(DEFAULT_MAX_MESSAGE_LENGTH)
        {
            initializeFromEnvironment();
        }
        ~Logger()
        {
            if (logFile_.is_open())
                logFile_.close();
        }
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        Level minLevel_;
        std::ofstream logFile_;
        mutable std::mutex mutex_;
        bool useSyslog_;
        bool showThreadId_;
        bool showFileLineFunc_;
        bool logFileFailed_;
        bool colorOutput_;
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
                              const char *file, int line, const char *func) const
        {
            // Pre-calculate approximate size to reduce reallocations
            std::string result;
            result.reserve(DEFAULT_MESSAGE_RESERVE_SIZE); // Most log messages are under 256 chars

            // Bold timestamp (only if color is enabled)
            if (colorOutput_)
                result += "\033[1m";
            result += "[";
            result += timestamp;
            result += "]";
            if (colorOutput_)
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
            if (colorOutput_)
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
