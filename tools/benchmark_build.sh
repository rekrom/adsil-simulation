#!/bin/bash

# Build Performance Benchmark Script
# Usage: ./benchmark_build.sh [--clean] [--no-ccache] [--incremental] [--help]

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m' 
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default settings
BUILD_TYPE="Release"
CLEAN_BUILD=false
USE_CCACHE=true
BUILD_DIR="build"
PARALLEL_JOBS=$(nproc)
INCREMENTAL_TEST=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --no-ccache)
            USE_CCACHE=false
            shift
            ;;
        --incremental)
            INCREMENTAL_TEST=true
            shift
            ;;
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        --help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --clean        Clean build directory before building"
            echo "  --no-ccache    Disable ccache for this build"
            echo "  --incremental  Test incremental build performance (make small change)"
            echo "  --build-type   Build type (Debug|Release|RelWithDebInfo|MinSizeRel)"
            echo "  --jobs N       Number of parallel jobs (default: $(nproc))"
            echo "  --help         Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                    # Normal build with ccache"
            echo "  $0 --clean           # Clean build with ccache"
            echo "  $0 --no-ccache       # Build without ccache"
            echo "  $0 --incremental     # Test incremental build speed"
            echo "  $0 --clean --no-ccache --build-type Debug"
            echo ""
            echo "ccache Performance Testing:"
            echo "  # Step 1: Establish cache"
            echo "  $0 --clean"
            echo "  # Step 2: Test incremental performance"  
            echo "  $0 --incremental"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Function to format time
format_time() {
    local seconds=$1
    local min=$((seconds / 60))
    local sec=$((seconds % 60))
    printf "%dm %02ds" $min $sec
}

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if ccache is available
if $USE_CCACHE && ! command -v ccache &> /dev/null; then
    print_warning "ccache not found, installing..."
    if command -v apt-get &> /dev/null; then
        sudo apt-get update && sudo apt-get install -y ccache
    elif command -v brew &> /dev/null; then
        brew install ccache
    else
        print_error "Cannot install ccache automatically. Please install manually or use --no-ccache"
        exit 1
    fi
fi

# Start benchmarking
print_status "Starting build benchmark..."
echo "Configuration:"
echo "  Build Type: $BUILD_TYPE"
echo "  Clean Build: $CLEAN_BUILD"
echo "  Use ccache: $USE_CCACHE"
echo "  Parallel Jobs: $PARALLEL_JOBS"
echo "  Incremental Test: $INCREMENTAL_TEST"
echo ""

TOTAL_START=$(date +%s)

# Handle incremental test
if $INCREMENTAL_TEST; then
    if [ ! -d "$BUILD_DIR" ]; then
        print_error "Build directory not found. Run a clean build first:"
        print_error "  $0 --clean"
        exit 1
    fi
    
    print_status "Making incremental change for performance test..."
    # Make a small change to force recompilation of main file
    TIMESTAMP=$(date +%s)
    echo "// Incremental build test - timestamp: $TIMESTAMP" >> apps/adsil_analyzer/main.cpp
    print_success "Added timestamp comment to main.cpp"
    echo ""
fi

# Clean build if requested
if $CLEAN_BUILD; then
    print_status "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Setup ccache if enabled
if $USE_CCACHE; then
    print_status "Setting up ccache..."
    ccache --version
    ccache --show-config | grep -E "(cache_dir|max_size|compression)"
    echo ""
    print_status "ccache statistics before build:"
    ccache --show-stats
    echo ""
fi

# Configure CMake
print_status "Configuring CMake..."
CONFIG_START=$(date +%s)

CMAKE_ARGS=(
    -B "$BUILD_DIR"
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
)

if $USE_CCACHE; then
    CMAKE_ARGS+=(
        -DCMAKE_C_COMPILER_LAUNCHER=ccache
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
    )
fi

cmake "${CMAKE_ARGS[@]}"
CONFIG_END=$(date +%s)
CONFIG_DURATION=$((CONFIG_END - CONFIG_START))

print_success "CMake configuration completed in $(format_time $CONFIG_DURATION)"

# Build
print_status "Building project..."
BUILD_START=$(date +%s)

cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel "$PARALLEL_JOBS"

BUILD_END=$(date +%s)
BUILD_DURATION=$((BUILD_END - BUILD_START))

print_success "Build completed in $(format_time $BUILD_DURATION)"

# Show ccache statistics after build
if $USE_CCACHE; then
    echo ""
    print_status "ccache statistics after build:"
    ccache --show-stats
    echo ""
fi

# Run tests
print_status "Running tests..."
TEST_START=$(date +%s)

cd "$BUILD_DIR"
ctest --output-on-failure -C "$BUILD_TYPE" --parallel "$PARALLEL_JOBS"
cd ..

TEST_END=$(date +%s)
TEST_DURATION=$((TEST_END - TEST_START))

print_success "Tests completed in $(format_time $TEST_DURATION)"

# Calculate total time
TOTAL_END=$(date +%s)
TOTAL_DURATION=$((TOTAL_END - TOTAL_START))

# Generate report
echo ""
echo "=========================================="
echo "           BENCHMARK RESULTS"
echo "=========================================="
echo ""
printf "%-20s %10s\n" "Stage" "Duration"
echo "----------------------------------------"
printf "%-20s %10s\n" "Configure" "$(format_time $CONFIG_DURATION)"
printf "%-20s %10s\n" "Build" "$(format_time $BUILD_DURATION)"
printf "%-20s %10s\n" "Test" "$(format_time $TEST_DURATION)"
echo "----------------------------------------"
printf "%-20s %10s\n" "TOTAL" "$(format_time $TOTAL_DURATION)"
echo "=========================================="

# Save results to file
RESULTS_FILE="benchmark_results_$(date +%Y%m%d_%H%M%S).json"
cat > "$RESULTS_FILE" << EOF
{
  "timestamp": "$(date -u +%Y-%m-%dT%H:%M:%SZ)",
  "build_type": "$BUILD_TYPE",
  "use_ccache": $USE_CCACHE,
  "clean_build": $CLEAN_BUILD,
  "parallel_jobs": $PARALLEL_JOBS,
  "system_info": {
    "os": "$(uname -s)",
    "arch": "$(uname -m)",
    "cpu_count": $(nproc),
    "memory_gb": $(free -g | grep '^Mem:' | awk '{print $2}')
  },
  "timing": {
    "configure_seconds": $CONFIG_DURATION,
    "build_seconds": $BUILD_DURATION,
    "test_seconds": $TEST_DURATION,
    "total_seconds": $TOTAL_DURATION
  }
}
EOF

print_success "Benchmark results saved to: $RESULTS_FILE"

# Show ccache hit rate if available
if $USE_CCACHE; then
    CACHE_HIT_RATE=$(ccache --show-stats | grep "cache hit rate" | awk '{print $4}' || echo "N/A")
    echo ""
    print_status "ccache hit rate: $CACHE_HIT_RATE"
fi

echo ""
print_success "Benchmark completed successfully!"
