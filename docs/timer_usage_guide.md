# Timer Class Usage Guide

The `core::Timer` class provides high-performance timing capabilities for measuring method execution times and code block performance.

## Features

- **Zero-overhead when disabled** (compile-time optimization)
- **Nanosecond precision** using `std::chrono::steady_clock`
- **Thread-safe** with thread-local storage
- **Multiple timing modes**: manual, scoped, block, and lambda-based
- **Performance statistics**: min, max, average, total, count
- **Memory efficient** with minimal allocations

## Quick Start

```cpp
#include "core/Timer.hpp"

void yourFunction() {
    TIMER_FUNCTION(); // Times the entire function

    // Your code here...
}
```

## Usage Patterns

### 1. Manual Timing

```cpp
TIMER_START("operation_name");
// ... code to measure ...
TIMER_END("operation_name");
```

### 2. Scoped Timing (RAII)

```cpp
void myFunction() {
    TIMER_SCOPE("myFunction");
    // Automatically timed until function exit
}
```

### 3. Block Timing

```cpp
TIMER_BLOCK("database_query") {
    // Your code block here
    database.executeQuery();
}
```

### 4. Function Timing

```cpp
void processData() {
    TIMER_FUNCTION(); // Uses function name automatically
    // Function implementation
}
```

### 5. Lambda/Callback Timing

```cpp
auto elapsed = core::Timer::measure("computation", []() {
    // Expensive computation
    performComplexCalculation();
});
```

## Performance Reporting

### Generate Report

```cpp
TIMER_REPORT(); // Prints formatted performance report
```

### Get Specific Statistics

```cpp
auto stats = core::Timer::getTimerStats("operation_name");
std::cout << "Average: " << stats.averageMs() << " ms\n";
std::cout << "Total: " << stats.totalMs() << " ms\n";
std::cout << "Count: " << stats.count << "\n";
```

### Reset All Timers

```cpp
TIMER_RESET(); // Clear all timing data
```

## Real-World Examples

### Method Performance Analysis

```cpp
class DataProcessor {
public:
    void processFile(const std::string& filename) {
        TIMER_FUNCTION();

        {
            TIMER_SCOPE("file_loading");
            loadFile(filename);
        }

        {
            TIMER_SCOPE("data_parsing");
            parseData();
        }

        {
            TIMER_SCOPE("computation");
            performComputation();
        }

        {
            TIMER_SCOPE("result_saving");
            saveResults();
        }
    }
};
```

### Loop Performance Monitoring

```cpp
void benchmarkAlgorithm() {
    for (int i = 0; i < 1000; ++i) {
        TIMER_SCOPE("algorithm_iteration");
        runAlgorithm();
    }

    // Get statistics for the repeated operation
    auto stats = core::Timer::getTimerStats("algorithm_iteration");
    std::cout << "Algorithm ran " << stats.count << " times\n";
    std::cout << "Average time: " << stats.averageMs() << " ms\n";
    std::cout << "Min time: " << stats.minMs() << " ms\n";
    std::cout << "Max time: " << stats.maxMs() << " ms\n";
}
```

### Conditional Timing

```cpp
void debugPerformance(bool enableTiming) {
    if (enableTiming) {
        TIMER_SCOPE("debug_operation");
    }

    // Your code here - timing is only active when enabled
}
```

## Performance Considerations

### Compile-Time Optimization

## Build Configuration

The Timer is **automatically disabled in Release builds** for zero overhead:

- **Debug builds**: `cmake .. -DCMAKE_BUILD_TYPE=Debug` → Timer **ENABLED**
- **Release builds**: `cmake .. -DCMAKE_BUILD_TYPE=Release` → Timer **DISABLED**

This is controlled by the `NDEBUG` preprocessor macro that CMake automatically defines in Release builds:

```cpp
#ifdef NDEBUG
    static constexpr bool TIMER_ENABLED = false;  // Release builds (zero overhead)
#else
    static constexpr bool TIMER_ENABLED = true;   // Debug builds (full timing)
#endif
```

### Verification

To verify Timer behavior in your build:

```bash
# Debug build - Timer enabled
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
# Your application will show timing output

# Release build - Timer disabled
cmake .. -DCMAKE_BUILD_TYPE=Release
make
# Your application will have zero timing overhead
```

### Manual Override

If you need to force enable/disable regardless of build type:

```cpp
// In Timer.hpp, replace the conditional with:
static constexpr bool TIMER_ENABLED = true;   // Always enabled
static constexpr bool TIMER_ENABLED = false;  // Always disabled
```

### Thread Safety

Each thread maintains its own timing data using `thread_local` storage, eliminating synchronization overhead.

### Memory Efficiency

- Uses static/thread-local storage to avoid dynamic allocations
- String keys are stored in `std::unordered_map` with small string optimization
- Minimal memory footprint per timer (~64 bytes)

### Precision

- Uses `std::chrono::steady_clock` for monotonic, high-resolution timing
- Nanosecond precision (actual resolution depends on system)
- Immune to system clock adjustments

## Integration with Logging

```cpp
void timedOperation() {
    TIMER_SCOPE("critical_operation");

    // Your operation
    auto result = performOperation();

    // Log timing results
    auto stats = core::Timer::getTimerStats("critical_operation");
    LOGGER_INFO_F("Operation completed in %.3f ms", stats.averageMs());
}
```

## Best Practices

1. **Use descriptive names**: `"database_query"` instead of `"op1"`
2. **Prefer scoped timing**: Less error-prone than manual start/end
3. **Aggregate similar operations**: Use the same timer name for repeated operations
4. **Profile in release builds**: Timing overhead is minimal but not zero
5. **Reset between test runs**: Use `TIMER_RESET()` for clean measurements
6. **Check thread safety**: Timer data is per-thread by default

## Macros Reference

| Macro                       | Description              |
| --------------------------- | ------------------------ |
| `TIMER_START(name)`         | Start manual timer       |
| `TIMER_END(name)`           | End manual timer         |
| `TIMER_SCOPE(name)`         | Scoped timer (RAII)      |
| `TIMER_FUNCTION()`          | Time current function    |
| `TIMER_BLOCK(name)`         | Time code block          |
| `TIMER_MEASURE(name, code)` | Time code snippet        |
| `TIMER_REPORT()`            | Print performance report |
| `TIMER_RESET()`             | Clear all timing data    |
