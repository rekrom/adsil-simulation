
#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>

#include <thread>
#include <syslog.h>

#define LOGGER_INFO(msg) core::Logger::getInstance().log("INFO", msg, __FILE__, __LINE__, __func__)
#define LOGGER_WARN(msg) core::Logger::getInstance().log("WARN", msg, __FILE__, __LINE__, __func__)
#define LOGGER_ERROR(msg) core::Logger::getInstance().log("ERROR", msg, __FILE__, __LINE__, __func__)
#define LOGGER_DEBUG(msg) core::Logger::getInstance().log("DEBUG", msg, __FILE__, __LINE__, __func__)
#define LOGGER_TRACE(msg) core::Logger::getInstance().log("TRACE", msg, __FILE__, __LINE__, __func__)

namespace core
{
    class Logger
    {
    public:
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
            else if (logFile_.is_open())
            {
                logFile_ << formatted << std::endl;
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

    private:
        Logger() : minLevel_(Level::TRACE), useSyslog_(false), showThreadId_(false), showFileLineFunc_(false) {}
        ~Logger()
        {
            if (logFile_.is_open())
                logFile_.close();
        }
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        Level minLevel_;
        std::ofstream logFile_;
        std::mutex mutex_;
        bool useSyslog_;
        bool showThreadId_;
        bool showFileLineFunc_;

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
            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
            ss << "." << std::setfill('0') << std::setw(3) << ms.count();
            return ss.str();
        }

        std::string getThreadId() const
        {
            std::stringstream ss;
            ss << std::this_thread::get_id();
            return ss.str();
        }

        std::string formatLog(const std::string &level, const std::string &msg,
                              const std::string &timestamp, const std::string &threadId,
                              const char *file, int line, const char *func) const
        {
            std::stringstream ss;
            ss << "\033[1m[" << timestamp << "]\033[0m ";
            if (showThreadId_)
                ss << "[" << threadId << "] ";

            // Only colorize the level part
            ss << getColor(level) << "[" << level << "]" << "\033[0m" << " ";

            if (showFileLineFunc_)
            {
                if (file)
                    ss << file << ":" << line << " (" << func << ") ";
            }
            ss << msg;
            return ss.str();
        }
    };
} // namespace core
