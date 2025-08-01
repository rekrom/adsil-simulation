#!/bin/bash

# Math Module Test Runner
# This script builds and runs all tests for the Math module

set -e  # Exit on any error

SCRIPT_DIR=$(dirname "$0")
PROJECT_ROOT=$(cd "$SCRIPT_DIR/../../.." && pwd)
BUILD_DIR="$PROJECT_ROOT/build"

echo "=== Math Module Test Runner ==="
echo "Project root: $PROJECT_ROOT"
echo "Build directory: $BUILD_DIR"
echo

# Ensure build directory exists
mkdir -p "$BUILD_DIR"

# Build the project
echo "Building project..."
cd "$BUILD_DIR"
cmake .. > /dev/null 2>&1
make -j$(nproc) > /dev/null 2>&1
echo "✅ Build completed successfully"
echo

# Run Math module tests
echo "Running Math module tests..."
echo "================================"

# List of Math module tests
MATH_TESTS=(
    "test_ConstantsTest"
    "test_VectorTest"
    "test_PointTest"
    "test_PointCloudTest"
    "test_MathHelperTest"
    "test_RotationUtilsTest"
    "test_MathIntegrationTest"
)

PASSED=0
FAILED=0
FAILED_TESTS=()

cd "$BUILD_DIR/bin"

for test in "${MATH_TESTS[@]}"; do
    echo "Running $test..."
    if ./"$test" > /dev/null 2>&1; then
        echo "✅ $test PASSED"
        PASSED=$((PASSED + 1))
    else
        echo "❌ $test FAILED"
        FAILED=$((FAILED + 1))
        FAILED_TESTS+=("$test")
    fi
done

echo
echo "================================"
echo "Test Summary:"
echo "✅ Passed: $PASSED"
echo "❌ Failed: $FAILED"
echo "Total: $((PASSED + FAILED))"

if [ $FAILED -gt 0 ]; then
    echo
    echo "Failed tests:"
    for test in "${FAILED_TESTS[@]}"; do
        echo "  - $test"
    done
    echo
    echo "To debug a failed test, run:"
    echo "  cd $BUILD_DIR/bin && ./<test_name>"
    exit 1
else
    echo
    echo "🎉 All Math module tests passed!"
    exit 0
fi
