# Development Tools

This directory contains essential tools for performance analysis and code quality checks.

## 🚀 Build Performance Tools

### `benchmark_build.sh`

Main script for comprehensive build performance analysis with ccache optimization.

**Quick Start:**

```bash
# Test ccache effectiveness
./tools/benchmark_build.sh --clean      # Establish cache
./tools/benchmark_build.sh --incremental # Test performance
```

**All Options:**

```bash
./tools/benchmark_build.sh [OPTIONS]

Options:
  --clean        Clean build directory before building
  --no-ccache    Disable ccache for this build
  --incremental  Test incremental build performance
  --build-type   Build type (Debug|Release|RelWithDebInfo|MinSizeRel)
  --jobs N       Number of parallel jobs
  --help         Show help message
```

**Features:**

- ✅ Real-time performance measurement
- ✅ ccache statistics and effectiveness tracking
- ✅ JSON reports for trend analysis
- ✅ Incremental build testing
- ✅ Colorized progress output

### `analyze_benchmarks.py`

Analyzes benchmark results to track performance trends over time.

**Usage:**

```bash
./tools/analyze_benchmarks.py              # Analyze all results
./tools/analyze_benchmarks.py --recent 5   # Show recent trends
```

**Features:**

- 📊 Performance trend analysis
- 📈 Statistical summaries (mean, median, std dev)
- 💡 Optimization recommendations
- 🔍 ccache effectiveness tracking

## 🔧 Code Quality Tools

### `detect_unused_features.py`

Analyzes C++ modular codebase for unused modules and missing dependencies.

**Usage:**

```bash
python3 tools/detect_unused_features.py
```

**Features:**

- 🔍 Unused modules detection
- 🔗 Missing dependency analysis
- 📊 Module usage statistics
- ⚠️ Linking issue identification

## 📈 Expected Performance Results

| Build Type      | Time      | ccache Hit Rate | Notes                           |
| --------------- | --------- | --------------- | ------------------------------- |
| **Cold build**  | 3-4 min   | 0%              | First build, establishing cache |
| **Warm build**  | 1-2 min   | 70-90%          | Full rebuild with cache         |
| **Incremental** | 30s-1 min | 90-98%          | Small changes only              |

## 🎯 CI Integration

The tools integrate with GitHub Actions workflows:

- **Main CI**: `cmake-single-platform.yml` - Real-time performance tracking
- **Weekly Analysis**: `enhanced-benchmark.yml` - Comprehensive performance analysis

Add to your CI pipeline:

```yaml
- name: Check for unused features
  run: ./tools/check_unused_features.sh
```

### With CMake

You can add a custom target to run the check:

```cmake
add_custom_target(check-unused
    COMMAND ${CMAKE_SOURCE_DIR}/tools/check_unused_features.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking for unused features"
)
```

## How It Works

1. **Module Discovery**: Scans the `modules/` directory for available modules
2. **CMake Analysis**: Parses `apps/adsil_analyzer/CMakeLists.txt` to find linked modules
3. **Include Analysis**: Traces `#include` statements to find actually used modules
4. **Dependency Graph**: Builds transitive dependency relationships
5. **Report Generation**: Compares linked vs used modules to find issues

## Common Issues Found

- **Missing Links**: Modules used through dependencies but not explicitly linked
- **Over-linking**: Modules linked but not actually used (increases binary size)
- **Dead Code**: Entire modules that can be removed from the build

## Benefits

- **Faster Builds**: Remove unused modules from compilation
- **Smaller Binaries**: Reduce linking overhead
- **Cleaner Code**: Identify and remove dead code
- **Better Dependencies**: Ensure proper module linking
