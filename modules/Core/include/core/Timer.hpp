#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <functional>

// Convenience macros for quick timing measurements
#define TIMER_START(name) core::Timer::start(name)
#define TIMER_END(name) core::Timer::end(name)
#define TIMER_SCOPE(name) core::ScopedTimer _scoped_timer_(name)
#define TIMER_FUNCTION() core::ScopedTimer _function_timer_(__func__)
#define TIMER_REPORT() core::Timer::report()
#define TIMER_RESET() core::Timer::reset()

// Advanced timing macros
#define TIMER_BLOCK(name) for (core::ScopedTimer _block_timer_(name); !_block_timer_.isDone(); _block_timer_.markDone())
#define TIMER_MEASURE(name, code) \
    do                            \
    {                             \
        core::Timer::start(name); \
        code;                     \
        core::Timer::end(name);   \
    } while (0)

namespace core
{
    /**
     * @brief High-performance Timer class for method and code block timing
     *
     * Features:
     * - Zero-overhead when disabled (compile-time optimization)
     * - Minimal memory footprint using static storage
     * - Thread-safe operations
     * - Nanosecond precision using steady_clock
     * - Multiple timing modes: manual, scoped, and lambda-based
     * - Accumulative timing for repeated operations
     * - Memory-efficient string key storage with small string optimization
     *
     * Usage patterns:
     * 1. Manual timing: Timer::start("operation") ... Timer::end("operation")
     * 2. Scoped timing: ScopedTimer timer("operation") or TIMER_SCOPE("operation")
     * 3. Block timing: TIMER_BLOCK("operation") { ... }
     * 4. Function timing: TIMER_FUNCTION() at function start
     * 5. Lambda timing: Timer::measure("operation", []() { ... })
     */
    class Timer
    {
    public:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;
        using Duration = std::chrono::nanoseconds;

        // Performance statistics for a timer
        struct TimerStats
        {
            Duration total{0};
            Duration min{Duration::max()};
            Duration max{Duration::min()};
            uint64_t count{0};

            void update(Duration elapsed)
            {
                total += elapsed;
                min = std::min(min, elapsed);
                max = std::max(max, elapsed);
                ++count;
            }

            double averageMs() const
            {
                return count > 0 ? (total.count() / static_cast<double>(count)) / 1e6 : 0.0;
            }

            double totalMs() const
            {
                return total.count() / 1e6;
            }

            double minMs() const
            {
                return min.count() / 1e6;
            }

            double maxMs() const
            {
                return max.count() / 1e6;
            }
        };

        /**
         * @brief Start timing for a named operation
         * @param name Unique name for the timer
         */
        static void start(const std::string &name)
        {
            if constexpr (TIMER_ENABLED)
            {
                getStartTimes()[name] = Clock::now();
            }
        }

        /**
         * @brief End timing for a named operation and record the duration
         * @param name Name of the timer to end
         * @return Duration in nanoseconds, or 0 if timer not found
         */
        static Duration end(const std::string &name)
        {
            if constexpr (TIMER_ENABLED)
            {
                auto endTime = Clock::now();
                auto &startTimes = getStartTimes();
                auto &stats = getStats();

                auto it = startTimes.find(name);
                if (it != startTimes.end())
                {
                    auto elapsed = std::chrono::duration_cast<Duration>(endTime - it->second);
                    stats[name].update(elapsed);
                    startTimes.erase(it);
                    return elapsed;
                }
            }
            return Duration{0};
        }

        /**
         * @brief Measure execution time of a function/lambda
         * @param name Timer name
         * @param func Function to measure
         * @return Duration in nanoseconds
         */
        template <typename Func>
        static Duration measure(const std::string &name, Func &&func)
        {
            if constexpr (TIMER_ENABLED)
            {
                auto startTime = Clock::now();
                func();
                auto endTime = Clock::now();
                auto elapsed = std::chrono::duration_cast<Duration>(endTime - startTime);
                getStats()[name].update(elapsed);
                return elapsed;
            }
            else
            {
                func();
                return Duration{0};
            }
        }

        /**
         * @brief Get statistics for a specific timer
         * @param name Timer name
         * @return TimerStats object, or default if not found
         */
        static TimerStats getTimerStats(const std::string &name)
        {
            if constexpr (TIMER_ENABLED)
            {
                auto &stats = getStats();
                auto it = stats.find(name);
                return it != stats.end() ? it->second : TimerStats{};
            }
            return TimerStats{};
        }

        /**
         * @brief Print performance report to console
         * @param sortByTotal If true, sort by total time; otherwise by average time
         */
        static void report(bool sortByTotal = true)
        {
            if constexpr (TIMER_ENABLED)
            {
                auto &stats = getStats();
                if (stats.empty())
                {
                    std::cout << "No timing data available\n";
                    return;
                }

                // Convert to vector for sorting
                std::vector<std::pair<std::string, TimerStats>> sortedStats(stats.begin(), stats.end());

                if (sortByTotal)
                {
                    std::sort(sortedStats.begin(), sortedStats.end(),
                              [](const auto &a, const auto &b)
                              {
                                  return a.second.total > b.second.total;
                              });
                }
                else
                {
                    std::sort(sortedStats.begin(), sortedStats.end(),
                              [](const auto &a, const auto &b)
                              {
                                  return a.second.averageMs() > b.second.averageMs();
                              });
                }

                std::cout << "\n"
                          << std::string(80, '=') << "\n";
                std::cout << "                           PERFORMANCE REPORT\n";
                std::cout << std::string(80, '=') << "\n";
                std::cout << std::left << std::setw(25) << "Timer Name"
                          << std::setw(10) << "Count"
                          << std::setw(12) << "Total (ms)"
                          << std::setw(12) << "Avg (ms)"
                          << std::setw(12) << "Min (ms)"
                          << std::setw(12) << "Max (ms)" << "\n";
                std::cout << std::string(80, '-') << "\n";

                for (const auto &[name, stat] : sortedStats)
                {
                    std::cout << std::left << std::setw(25) << name
                              << std::setw(10) << stat.count
                              << std::setw(12) << std::fixed << std::setprecision(3) << stat.totalMs()
                              << std::setw(12) << std::fixed << std::setprecision(3) << stat.averageMs()
                              << std::setw(12) << std::fixed << std::setprecision(3) << stat.minMs()
                              << std::setw(12) << std::fixed << std::setprecision(3) << stat.maxMs() << "\n";
                }
                std::cout << std::string(80, '=') << "\n\n";
            }
        }

        /**
         * @brief Reset all timing data
         */
        static void reset()
        {
            if constexpr (TIMER_ENABLED)
            {
                getStartTimes().clear();
                getStats().clear();
            }
        }

        /**
         * @brief Get total number of active timers
         */
        static size_t getActiveTimerCount()
        {
            if constexpr (TIMER_ENABLED)
            {
                return getStartTimes().size();
            }
            return 0;
        }

// Compile-time switch to enable/disable timing
// Automatically disabled in Release builds (when NDEBUG is defined) for zero overhead
#ifdef NDEBUG
        static constexpr bool TIMER_ENABLED = false; // Release builds (NDEBUG defined)
#else
        static constexpr bool TIMER_ENABLED = true; // Debug builds
#endif

        // Thread-local storage for better performance in multi-threaded scenarios
        static std::unordered_map<std::string, TimePoint> &getStartTimes()
        {
            static thread_local std::unordered_map<std::string, TimePoint> startTimes;
            return startTimes;
        }

        static std::unordered_map<std::string, TimerStats> &getStats()
        {
            static thread_local std::unordered_map<std::string, TimerStats> stats;
            return stats;
        }
    };

    /**
     * @brief RAII-based scoped timer for automatic timing
     *
     * Automatically starts timing on construction and ends on destruction.
     * Perfect for timing function calls or code blocks.
     */
    class ScopedTimer
    {
    public:
        explicit ScopedTimer(const std::string &name)
            : name_(name), startTime_(Timer::Clock::now()), done_(false)
        {
            // No need to call Timer::start() - we handle timing directly for better performance
        }

        ~ScopedTimer()
        {
            if (!done_)
            {
                end();
            }
        }

        // Non-copyable, movable
        ScopedTimer(const ScopedTimer &) = delete;
        ScopedTimer &operator=(const ScopedTimer &) = delete;
        ScopedTimer(ScopedTimer &&) = default;
        ScopedTimer &operator=(ScopedTimer &&) = default;

        /**
         * @brief Manually end the timer (useful for TIMER_BLOCK macro)
         */
        Timer::Duration end()
        {
            if constexpr (Timer::TIMER_ENABLED)
            {
                if (!done_)
                {
                    auto endTime = Timer::Clock::now();
                    auto elapsed = std::chrono::duration_cast<Timer::Duration>(endTime - startTime_);
                    Timer::getStats()[name_].update(elapsed);
                    done_ = true;
                    return elapsed;
                }
            }
            return Timer::Duration{0};
        }

        /**
         * @brief Check if timer is finished (for TIMER_BLOCK macro)
         */
        bool isDone() const { return done_; }

        /**
         * @brief Mark timer as done (for TIMER_BLOCK macro)
         */
        void markDone() { done_ = true; }

    private:
        std::string name_;
        Timer::TimePoint startTime_;
        bool done_;
    };

} // namespace core
