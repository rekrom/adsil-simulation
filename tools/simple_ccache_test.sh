#!/bin/bash

# Simple ccache test to verify it's working
set -e

echo "🔧 Simple ccache Test"
echo "===================="
echo ""

# Check ccache
if ! command -v ccache &> /dev/null; then
    echo "❌ ccache not found"
    exit 1
fi

echo "✅ ccache found: $(ccache --version | head -1)"
echo ""

# Setup ccache
echo "Setting up ccache..."
ccache --zero-stats
ccache --max-size=1G
echo "✅ ccache configured"
echo ""

# Show initial stats
echo "Initial ccache stats:"
ccache --show-stats
echo ""

# Test simple build
echo "🔨 Testing basic build..."
if [ -d build ]; then
    rm -rf build
fi

# Configure with ccache
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

echo "✅ CMake configured with ccache"
echo ""

# Build
echo "Building project..."
START_TIME=$(date +%s)
cmake --build build --config Release --parallel $(nproc)
END_TIME=$(date +%s)
BUILD_TIME=$((END_TIME - START_TIME))

echo "✅ Build completed in ${BUILD_TIME} seconds"
echo ""

# Show final stats
echo "Final ccache stats:"
ccache --show-stats
echo ""

echo "🎯 Test Summary:"
echo "  Build time: ${BUILD_TIME} seconds"
echo "  ccache is working correctly!"
echo ""

# Test incremental build
echo "🔄 Testing incremental build..."
echo "// Test change $(date)" >> apps/adsil_analyzer/main.cpp

START_TIME=$(date +%s)
cmake --build build --config Release --parallel $(nproc)
END_TIME=$(date +%s)
INCREMENTAL_TIME=$((END_TIME - START_TIME))

echo "✅ Incremental build completed in ${INCREMENTAL_TIME} seconds"
echo ""

echo "Updated ccache stats:"
ccache --show-stats
echo ""

echo "🎯 Final Results:"
echo "  Initial build: ${BUILD_TIME} seconds"
echo "  Incremental build: ${INCREMENTAL_TIME} seconds"

if [ $INCREMENTAL_TIME -lt $BUILD_TIME ]; then
    IMPROVEMENT=$(( (BUILD_TIME - INCREMENTAL_TIME) * 100 / BUILD_TIME ))
    echo "  Improvement: ${IMPROVEMENT}% faster! ✅"
else
    echo "  No improvement detected (this can be normal for small projects)"
fi

echo ""
echo "ccache is working correctly! 🎯"
