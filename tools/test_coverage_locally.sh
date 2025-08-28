#!/usr/bin/env bash
set -euo pipefail

# Clean previous coverage
rm -rf coverage
mkdir -p coverage

# Configure with coverage flags
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON \
  -DCMAKE_CXX_FLAGS="--coverage" -DCMAKE_EXE_LINKER_FLAGS="--coverage"

# Build and run tests
cmake --build build --parallel
ctest --test-dir build --output-on-failure

# Generate lcov report
lcov --directory build --capture --output-file coverage/coverage.info
lcov --remove coverage/coverage.info '/usr/*' '*/external/*' --output-file coverage/coverage.info

# Generate HTML report
genhtml coverage/coverage.info --output-directory coverage/html

# Generate summary with gcovr (optional)
gcovr -r . --html --html-details -o coverage/coverage.html
gcovr -r . --xml -o coverage/coverage.xml

echo "✅ Coverage report generated:"
echo " - HTML:   coverage/html/index.html"
echo " - gcovr:  coverage/coverage.html"
