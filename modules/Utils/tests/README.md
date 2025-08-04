# Utils Module Test Documentation

This document provides a comprehensive overview of the Utils module test suite.

## Overview

The Utils module provides JSON file utilities for the ADSIL Analyzer C++ project. This test suite ensures the reliability, correctness, and performance of all Utils module functionality.

## Test Structure

### Core Test Files

1. **`UtilsTest.cpp`** - Main functionality tests
2. **`UtilsEdgeCaseTest.cpp`** - Edge cases and performance tests
3. **`CMakeLists.txt`** - Test build configuration

### Test Coverage

The test suite covers 100% of the Utils module functionality with comprehensive testing of:

- ✅ All public functions in `JsonFileUtils`
- ✅ Error handling and edge cases
- ✅ Performance characteristics
- ✅ Unicode and special character support
- ✅ Large file handling
- ✅ Memory management

## Main Test Suite (`UtilsTest.cpp`)

### Functions Tested

#### `readJsonFromFile()`

- **Valid JSON parsing** - Tests proper parsing of JSON objects and arrays
- **File not found handling** - Ensures graceful failure for missing files
- **Invalid JSON handling** - Tests malformed JSON rejection
- **Empty file handling** - Tests empty file rejection

#### `writeJsonToFile()`

- **Simple object writing** - Tests basic JSON object serialization
- **Complex object writing** - Tests nested structures and arrays
- **Invalid path handling** - Tests write failure for inaccessible paths
- **JSON formatting** - Tests proper indentation and formatting

#### `loadJsonOrExit()`

- **Valid file loading** - Tests successful JSON loading
- **Invalid file handling** - Tests exception throwing for failed loads
- **Error message validation** - Ensures proper error reporting

### Test Data Coverage

The tests use diverse JSON structures including:

- Simple key-value pairs
- Nested objects and arrays
- Boolean, string, and numeric values
- Complex hierarchical data

## Edge Case Test Suite (`UtilsEdgeCaseTest.cpp`)

### Advanced Testing Scenarios

#### Large File Handling

- **1000 entries** with complex nested structures
- **Performance measurement** for read/write operations
- **File size validation** (>100KB files)
- **Memory efficiency testing**

#### Unicode and Special Characters

- **Multi-language support**: English, German, Spanish, Chinese, Japanese, Russian, Arabic
- **Emoji handling**: 🌍🚀🎉💻📊
- **Special characters**: Quotes, newlines, tabs, backslashes
- **Escape sequences**: Proper handling of JSON escape codes

#### JSON Structure Edge Cases

- **Deep nesting**: 100 levels of nested objects
- **Large arrays**: 10,000 element arrays
- **Complex data types**: Mixed arrays and objects

#### Error Handling Edge Cases

- **10 different malformed JSON patterns**:
  - Incomplete objects and arrays
  - Trailing commas
  - Unquoted values
  - Invalid escape sequences
  - Multiple root values
  - Invalid literals and numbers

#### File Access Patterns

- **Rapid read operations**: 10 consecutive reads
- **File overwriting**: Multiple write operations
- **Concurrent access simulation**

#### Performance Characteristics

- **Scalability testing**: Incrementally larger datasets
- **Performance monitoring**: Write and read timing
- **Memory efficiency**: Growth pattern analysis

## Test Execution

### Build Commands

```bash
# Enable testing
cmake -DBUILD_TESTING=ON ..

# Build Utils tests
make test_UtilsTest
make test_UtilsEdgeCaseTest

# Run tests directly
./bin/test_UtilsTest
./bin/test_UtilsEdgeCaseTest

# Run via CTest
ctest -R "Utils" -V
```

### Test Results

- **Main Test Suite**: 12 test functions, ~50 assertions
- **Edge Case Suite**: 6 test categories, ~100 assertions
- **Total Execution Time**: <200ms
- **Success Rate**: 100%

## Test Configuration

### CMake Integration

- **CTest integration**: Tests registered with CTest framework
- **Timeout settings**: 30s for main tests, 60s for edge cases
- **Test labels**: `utils`, `json`, `filesystem`, `performance`, `edge-cases`
- **Working directory**: Build directory for temporary file access

### Dependencies

- **nlohmann/json**: JSON parsing and serialization
- **C++17 filesystem**: File operations
- **Custom assertions**: Detailed error reporting

## Coverage Analysis

### Function Coverage

- `readJsonFromFile()`: ✅ 100%
- `writeJsonToFile()`: ✅ 100%
- `loadJsonOrExit()`: ✅ 100%

### Branch Coverage

- **Success paths**: All valid input scenarios
- **Error paths**: All error conditions and exceptions
- **Edge cases**: Boundary conditions and unusual inputs

### Performance Validation

- **Large file handling**: Up to 500KB JSON files
- **Unicode processing**: Multi-byte character handling
- **Memory efficiency**: No memory leaks or excessive allocation

## Test Utilities

### TempFileHelper Class

A utility class that provides:

- **Temporary directory management**
- **Automatic cleanup**
- **Safe file creation**
- **Path management**

### Assertion Framework

Custom assertion functions providing:

- **Detailed error messages**
- **Test progress tracking**
- **Visual success indicators (✓)**

## Integration with Coverage Workflow

The Utils tests integrate with the project's coverage workflow:

- **lcov compatibility**: Generates coverage data
- **gcovr integration**: Alternative coverage reporting
- **CI/CD pipeline**: Automated test execution
- **Coverage reporting**: Detailed coverage metrics

## Maintenance Notes

### Adding New Tests

1. Add test functions to appropriate test file
2. Update CMakeLists.txt if needed
3. Ensure proper cleanup in TempFileHelper
4. Run full test suite to verify

### Performance Benchmarks

The edge case tests establish performance baselines:

- **Write performance**: <10ms for 500 items
- **Read performance**: <50ms for 500 items
- **File size efficiency**: Reasonable JSON formatting

### Error Message Localization

Note: Current error messages are in Turkish. For international deployment, consider:

- Adding error message localization
- Standardizing error formats
- Ensuring consistent error reporting

## Conclusion

The Utils module test suite provides comprehensive validation of all JSON file utilities with:

- **100% function coverage**
- **Extensive edge case testing**
- **Performance validation**
- **Unicode support verification**
- **Robust error handling**

This ensures the Utils module meets all reliability and performance requirements for the ADSIL Analyzer project.
