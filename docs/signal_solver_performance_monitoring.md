# Signal Solver Performance Monitoring

The Timer class has been integrated into `SimulationManager` to specifically measure the performance of your signal solver operations.

## What's Been Instrumented

### 1. Signal Solver Performance (Line 298)

The critical line you highlighted:

```cpp
detectedPointCloudEntity_->setPointCloud(signalSolver_->solve());
```

Has been enhanced with precise timing:

```cpp
// Time the critical signal solver operation
std::shared_ptr<math::PointCloud> pointCloud;
core::Timer::measure("SignalSolver_solve", [&]() {
    pointCloud = signalSolver_->solve();
});

// Time the point cloud update operation
core::Timer::measure("PointCloudEntity_setPointCloud", [&]() {
    detectedPointCloudEntity_->setPointCloud(pointCloud);
});
```

### 2. Simulation Loop Breakdown

- **Frame-level timing**: `SimulationLoop_Frame` - Total time per frame
- **Update timing**: `SimulationLoop_Update` - State update operations
- **Render timing**: `SimulationLoop_Render` - Rendering operations
- **Signal processing**: `SignalProcessing_Total` - Complete signal processing pipeline

## Performance Monitoring Features

### Automatic Statistics Collection

Every call to `signalSolver_->solve()` is now automatically:

- Timed with nanosecond precision
- Accumulated for statistical analysis (min, max, average, total)
- Counted for frequency analysis

### Performance Reporting Methods

```cpp
// Get detailed performance report
simulationManager.reportPerformanceStats();

// Reset statistics for clean measurement
simulationManager.resetPerformanceStats();
```

### Real-time Performance Analysis

The `reportPerformanceStats()` method provides:

- **Signal Solver Analysis**: Average, min, max solve times
- **Performance Impact**: Percentage of frame time spent in signal solver
- **Frame Rate Analysis**: Average FPS and frame times
- **Call Frequency**: Number of solver calls

## Usage Examples

### 1. Measure Signal Solver During Runtime

```cpp
// After running simulation for a while
simulationManager.reportPerformanceStats();
```

### 2. Compare Before/After Optimizations

```cpp
// Before optimization
simulationManager.resetPerformanceStats();
// ... run simulation for N frames ...
auto stats1 = core::Timer::getTimerStats("SignalSolver_solve");

// After optimization
simulationManager.resetPerformanceStats();
// ... run simulation for N frames ...
auto stats2 = core::Timer::getTimerStats("SignalSolver_solve");

// Compare performance
LOGGER_INFO_F("Performance improvement: %.2f%%",
    ((stats1.averageMs() - stats2.averageMs()) / stats1.averageMs()) * 100.0);
```

### 3. Performance Profiling in Debug Mode

```cpp
// In your main loop, periodically report stats
static int frameCounter = 0;
if (++frameCounter % 1000 == 0) {  // Every 1000 frames
    simulationManager.reportPerformanceStats();
    simulationManager.resetPerformanceStats();
}
```

### 4. Conditional Performance Logging

```cpp
// Only log if signal solver is taking too long
auto solverStats = core::Timer::getTimerStats("SignalSolver_solve");
if (solverStats.averageMs() > 10.0) {  // If > 10ms average
    LOGGER_WARN_F("Signal solver performance degraded: %.3f ms average",
                  solverStats.averageMs());
}
```

## Expected Output Example

When you call `reportPerformanceStats()`, you'll see output like:

```
INFO: === SIMULATION PERFORMANCE REPORT ===

================================================================================
                           PERFORMANCE REPORT
================================================================================
Timer Name               Count     Total (ms)  Avg (ms)    Min (ms)    Max (ms)
--------------------------------------------------------------------------------
SimulationLoop_Frame     1000      16667.123   16.667      15.234      23.456
SignalSolver_solve       1000      8234.567    8.235       7.123       12.345
SimulationLoop_Update    1000      4123.789    4.124       3.567       6.789
SimulationLoop_Render    1000      3456.789    3.457       2.890       5.123
SignalProcessing_Total   1000      8456.123    8.456       7.234       12.567
PointCloudEntity_setPoin 1000      221.556     0.222       0.156       0.456
================================================================================

INFO: Signal Solver Performance Analysis:
INFO:   - Average solve time: 8.235 ms
INFO:   - Min solve time: 7.123 ms
INFO:   - Max solve time: 12.345 ms
INFO:   - Total solve calls: 1000
INFO:   - Total solve time: 8234.567 ms
INFO:   - Signal solver represents 49.4% of total frame time

INFO: Frame Performance:
INFO:   - Average frame time: 16.667 ms (60.0 FPS)
INFO:   - Total frames processed: 1000

INFO: === END PERFORMANCE REPORT ===
```

## Performance Optimization Insights

Based on the timing data, you can:

1. **Identify bottlenecks**: See if signal solver is the main performance issue
2. **Track regressions**: Monitor if performance degrades over time
3. **Validate optimizations**: Measure improvement after code changes
4. **Frame rate analysis**: Understand what's limiting your FPS
5. **Resource allocation**: See how much time each component consumes

## Build Configuration

The Timer can be completely disabled in production builds by setting:

```cpp
static constexpr bool TIMER_ENABLED = false;  // In Timer.hpp
```

This results in zero runtime overhead when timing is not needed.
