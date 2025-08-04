# Code Coverage Setup for ADSIL Simulation

## Overview

This document explains the code coverage setup for the ADSIL simulation project, including GitHub Actions CI integration with Codecov.

## 📊 Coverage Tracking Components

### 1. README Coverage Badge

The project README displays a live coverage badge that automatically updates with each CI run:

- **Badge Location**: Top of README.md after the title
- **Badge URL**: `https://codecov.io/gh/rekrom/adsil-simulation/graph/badge.svg`
- **Update Mechanism**: Automatically updates when coverage data is uploaded to Codecov
- **Badge Colors**:
  - 🟢 Green: ≥70% coverage
  - 🟡 Yellow: 50-69% coverage
  - 🔴 Red: <50% coverage

### 2. GitHub Actions Workflow (`.github/workflows/coverage.yml`)

- **Triggers**: On push to `main`/`develop` branches and pull requests
- **Build Configuration**: Debug build with coverage flags (`--coverage`, `-fprofile-arcs`, `-ftest-coverage`)
- **Test Execution**: Runs all module tests including comprehensive adapter test suite
- **Coverage Generation**: Creates both lcov and gcovr reports
- **Codecov Upload**: Automatically uploads coverage data to Codecov dashboard
- **Artifacts**: Saves coverage reports for download and inspection

### 2. Codecov Configuration (`codecov.yml`)

- **Coverage Targets**: 70% minimum coverage requirement
- **Module-Specific Flags**: Individual tracking for each module (Adapter, Math, Geometry, etc.)
- **Ignore Patterns**: Excludes external libraries, tests, and build artifacts
- **Comment Integration**: Automatic PR comments with coverage summaries

### 3. Local Testing Script (`test_coverage_locally.sh`)

- **Prerequisites Check**: Validates lcov, gcovr, and cmake availability
- **Local Coverage Generation**: Builds with coverage flags and runs tests
- **Report Generation**: Creates HTML and XML coverage reports
- **CI Validation**: Tests the workflow locally before pushing

## 🚀 Quick Start

### For Contributors

1. **Install Coverage Tools** (Ubuntu/Debian):

   ```bash
   sudo apt-get install lcov gcovr
   ```

2. **Test Coverage Locally**:

   ```bash
   ./test_coverage_locally.sh
   ```

3. **Update README Badge (Optional)**:

   ```bash
   # Local README badge update with current coverage
   ./update_coverage_readme.sh
   ```

4. **View HTML Report**:
   ```bash
   # After running local test
   open coverage/html/index.html
   ```

### For CI/CD

The coverage workflow runs automatically on:

- Push to `main` or `develop` branches
- Pull requests to `main`

## 📈 Coverage Reports

### Codecov Dashboard

- **Organization**: Your Codecov organization
- **Repository**: adsil-simulation
- **Access**: Automatic after first successful workflow run

### Local Reports

After running `./test_coverage_locally.sh`:

- **HTML Report**: `coverage/html/index.html`
- **XML Report**: `coverage/coverage.xml` (Codecov format)
- **LCOV Report**: `coverage/coverage.info` (lcov format)

## 🎯 Coverage Targets

### Current Targets

- **Overall Project**: 70% minimum coverage
- **Per Module**: 70% minimum coverage
- **Patch Coverage**: 70% for new code changes

### Module Breakdown

| Module         | Description                     | Test Coverage                    |
| -------------- | ------------------------------- | -------------------------------- |
| **Adapter**    | JSON serialization system       | ✅ Comprehensive (9 test suites) |
| **Math**       | Mathematical operations         | ✅ Good coverage                 |
| **Geometry**   | Geometric shapes and operations | ✅ Basic coverage                |
| **Spatial**    | Spatial transformations         | ✅ Basic coverage                |
| **Vehicle**    | Vehicle representations         | ✅ Basic coverage                |
| **Simulation** | Simulation framework            | ✅ Basic coverage                |
| **Core**       | Core utilities                  | ✅ Basic coverage                |
| **Utils**      | Utility functions               | ✅ Basic coverage                |
| **Viewer**     | Visualization components        | ⚠️ Limited coverage              |

## 🔧 Configuration Details

### Coverage Flags

```cmake
CMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage"
CMAKE_C_FLAGS="--coverage -fprofile-arcs -ftest-coverage"
CMAKE_EXE_LINKER_FLAGS="--coverage"
CMAKE_SHARED_LINKER_FLAGS="--coverage"
```

### Excluded from Coverage

- External libraries (`external/`)
- Test files (`*Test.cpp`, `*/tests/`)
- Build artifacts (`build/`)
- Documentation (`docs/`)
- Configuration files (`*.yml`, `*.json`, `CMakeLists.txt`)

## 📝 Adding Tests for New Code

### 1. Create Test File

```cpp
// modules/YourModule/tests/YourFeatureTest.cpp
#include <YourModule/YourFeature.hpp>
#include <cassert>

void test_your_feature() {
    // Test implementation
    assert(/* your assertions */);
}

int main() {
    test_your_feature();
    return 0;
}
```

### 2. Update CMakeLists.txt

The test will be automatically discovered by the existing CMake configuration.

### 3. Run Coverage Check

```bash
./test_coverage_locally.sh
```

## 🚨 Troubleshooting

### Common Issues

1. **Low Coverage Warning**

   ```
   ❌ Coverage X% below threshold of 70%
   ```

   **Solution**: Add more tests or review test quality

2. **Missing Coverage Tools**

   ```bash
   sudo apt-get install lcov gcovr build-essential
   ```

3. **Build Failures with Coverage**

   - Ensure Debug build configuration
   - Check that coverage flags are properly set
   - Verify all dependencies are installed

4. **Codecov Upload Failures**
   - Check that `CODECOV_TOKEN` secret is set in repository
   - Verify network connectivity in CI environment
   - Review Codecov configuration syntax

### Debug Coverage Generation

```bash
# Manual coverage generation
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage"
cmake --build build
./build/bin/test_*

# Generate reports
lcov --capture --directory build --output-file coverage.info
gcovr --xml coverage.xml
```

## 📊 Understanding Coverage Metrics

### Line Coverage

Percentage of executable lines that were executed during testing.

### Branch Coverage

Percentage of conditional branches (if/else, switch cases) that were tested.

### Function Coverage

Percentage of functions that were called during testing.

## 🔄 Continuous Improvement

### Coverage Goals

1. **Short Term**: Maintain 70% overall coverage
2. **Medium Term**: Achieve 80% coverage for core modules
3. **Long Term**: Reach 90% coverage with comprehensive integration tests

### Best Practices

- Write tests for new features before implementation (TDD)
- Focus on testing edge cases and error conditions
- Regular coverage review in code reviews
- Monitor coverage trends over time

## 📞 Support

For coverage-related questions:

1. Check this documentation first
2. Run local coverage tests to isolate issues
3. Review GitHub Actions logs for CI failures
4. Check Codecov dashboard for detailed reports

## 🔗 Useful Links

- [Codecov Documentation](https://docs.codecov.io/)
- [LCOV Documentation](http://ltp.sourceforge.net/coverage/lcov.php)
- [GCOVR Documentation](https://gcovr.com/)
- [GitHub Actions Coverage Best Practices](https://docs.github.com/en/actions)
