# CI Pipeline Performance Enhancements

This document outlines the improvements made to the CI pipeline to reduce build times and track performance metrics.

## 🚀 Key Improvements

### 1. Enhanced ccache Configuration

**Before:**

- ccache was configured but cache directory wasn't properly cached
- Only build directory was cached, not the actual compilation cache
- No cache statistics or monitoring

**After:**

- Dedicated ccache directory (`$CCACHE_DIR`) with proper caching
- Cache key includes source files for better cache invalidation
- Maximum cache size configured (1GB)
- Statistics displayed before and after build
- Zero stats reset for accurate measurements

### 2. Optimized Caching Strategy

**Multiple cache layers:**

1. **ccache files**: Caches compiled object files across builds
2. **CMake build directory**: Caches CMake configuration and intermediate files
3. **Hierarchical cache keys**: Falls back to partial matches for better cache hits

**Cache key strategy:**

- Primary key: OS + build type + hash of all CMake and source files
- Fallback keys: OS + build type, then OS only

### 3. Parallel Build Optimization

- Enabled parallel compilation with `--parallel $(nproc)`
- Parallel testing with `ctest --parallel $(nproc)`
- Export compile commands for better tooling integration

### 4. Comprehensive Benchmarking

#### CI Integration

- **Real-time benchmarks**: Every CI run shows detailed timing breakdown
- **Step-by-step timing**: Dependencies, configure, build, test phases tracked
- **Performance dashboard**: Results displayed in GitHub Actions summary
- **ccache metrics**: Hit rate, cache size, effectiveness tracking

#### Local Benchmarking Tools

- **`benchmark_build.sh`**: Local performance testing script
- **`analyze_benchmarks.py`**: Historical performance analysis
- **JSON output**: Machine-readable benchmark data for tracking

## 📊 Expected Performance Improvements

### First Build (Cold Cache)

- **Dependencies**: ~1 minute (unchanged)
- **Configure**: ~10-30 seconds
- **Build**: 2-4 minutes (depending on parallelization)
- **Test**: ~30 seconds

### Subsequent Builds (Warm Cache)

- **Dependencies**: ~30 seconds (cached packages)
- **Configure**: ~5-15 seconds (cached CMake)
- **Build**: **30 seconds - 2 minutes** (ccache effectiveness)
- **Test**: ~30 seconds

### Estimated Total Time Reduction

- **Cold build**: 4-6 minutes (vs. previous 4+ minutes)
- **Warm build**: **2-3 minutes** (vs. previous 4+ minutes)
- **Change-only builds**: **1-2 minutes** (significant improvement)

## 🔧 CI Workflow Features

### Enhanced Main Workflow (`cmake-single-platform.yml`)

- ✅ Proper ccache setup and caching
- ✅ Build-time benchmarking
- ✅ Performance reporting in PR summaries
- ✅ Parallel builds and tests
- ✅ ccache statistics monitoring

### Dedicated Benchmark Workflow (`benchmark.yml`)

- ✅ Matrix builds (with/without ccache comparison)
- ✅ Detailed performance profiling
- ✅ Weekly scheduled runs for trend tracking
- ✅ Manual trigger capability
- ✅ Comprehensive performance reports

## 🎯 Monitoring and Alerts

### GitHub Actions Summary

Each CI run now displays:

```
🚀 CI Pipeline Benchmark Report

| Stage | Duration | Status |
|-------|----------|--------|
| Dependencies | 0m 45s | ✅ |
| CMake Configure | 0m 12s | ✅ |
| Build | 1m 23s | ✅ |
| Tests | 0m 31s | ✅ |
| Total | 2m 51s | ✅ |

🎯 ccache Statistics
Cache hit rate: 78.5%
```

### Performance Tracking

- Historical performance data collection
- Trend analysis across commits
- Regression detection
- Cache effectiveness monitoring

## 🏃‍♂️ Quick Start

### Run Local Benchmark

```bash
# Standard benchmark with ccache
./tools/benchmark_build.sh

# Compare with and without ccache
./tools/benchmark_build.sh --clean --no-ccache
./tools/benchmark_build.sh --clean

# Analyze results
./tools/analyze_benchmarks.py
```

### Trigger CI Benchmark

- Push to `main` branch (automatic)
- Create pull request (automatic)
- Manual trigger via GitHub Actions UI
- Weekly scheduled runs (Mondays 2 AM UTC)

## 🔍 Troubleshooting

### If builds are still slow:

1. Check ccache hit rate in CI logs
2. Verify cache keys are working properly
3. Monitor cache size and storage limits
4. Review parallel job configuration

### Local performance issues:

1. Run `./tools/benchmark_build.sh --clean --no-ccache` vs with ccache
2. Check ccache configuration: `ccache --show-config`
3. Monitor system resources during build
4. Use `./tools/analyze_benchmarks.py` for trend analysis

## 📈 Future Improvements

- **Unity builds**: Combine multiple source files for faster compilation
- **Precompiled headers**: Cache commonly used headers
- **Distributed compilation**: Scale across multiple runners
- **Build cache artifacts**: Share build artifacts between workflows
- **Incremental builds**: Smarter dependency tracking
