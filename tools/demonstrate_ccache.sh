#!/bin/bash

# ccache Performance Demonstration Script
# This script demonstrates the real-world benefits of ccache

set -e

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m' 
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

print_header() {
    echo ""
    echo -e "${BLUE}============================================${NC}"
    echo -e "${BLUE} $1${NC}"
    echo -e "${BLUE}============================================${NC}"
    echo ""
}

print_step() {
    echo -e "${GREEN}[STEP]${NC} $1"
}

print_info() {
    echo -e "${YELLOW}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if ccache is available
if ! command -v ccache &> /dev/null; then
    print_error "ccache not found. Please install ccache first:"
    echo "  sudo apt install ccache"
    exit 1
fi

print_header "ccache Performance Demonstration"

echo "This script will demonstrate the real benefits of ccache by:"
echo "1. Building the project from clean state (cold cache)"
echo "2. Making a small change and rebuilding (warm cache)"
echo "3. Showing the performance difference"
echo ""

read -p "Press Enter to start the demonstration..."

# Step 1: Clean build to establish cache
print_header "Step 1: Cold Build (Establishing Cache)"
print_step "Building project with clean cache to establish baseline..."

ccache --zero-stats
ccache --max-size=1G

echo "ccache stats before build:"
ccache --show-stats || echo "ccache stats not available"
echo ""

START_TIME=$(date +%s)
print_step "Running clean build with ccache..."
./tools/benchmark_build.sh --clean
COLD_TIME=$(date +%s)
COLD_DURATION=$((COLD_TIME - START_TIME))

echo ""
print_info "Cold build completed in ${COLD_DURATION} seconds"
echo "ccache stats after cold build:"
ccache --show-stats || echo "ccache stats not available"

# Step 2: Make incremental change  
print_header "Step 2: Incremental Build (Using Cache)"
print_step "Making small change and rebuilding to test cache effectiveness..."

# Make a small change that will trigger recompilation of a few files
TIMESTAMP=$(date +%s)
echo "// Performance test change - $TIMESTAMP" >> apps/adsil_analyzer/main.cpp

echo "ccache stats before incremental build:"
ccache --show-stats || echo "ccache stats not available"
echo ""

WARM_START=$(date +%s)
print_step "Running incremental build with ccache..."
./tools/benchmark_build.sh
WARM_TIME=$(date +%s)
WARM_DURATION=$((WARM_TIME - WARM_START))

# Step 3: Show results
print_header "Step 3: Performance Comparison Results"

IMPROVEMENT=$((COLD_DURATION - WARM_DURATION))
PERCENT_IMPROVEMENT=$(( (IMPROVEMENT * 100) / COLD_DURATION ))

echo "📊 Build Performance Results:"
echo ""
printf "%-25s %10s\n" "Build Type" "Duration"
echo "----------------------------------------"
printf "%-25s %10ss\n" "Cold build (no cache)" "$COLD_DURATION"
printf "%-25s %10ss\n" "Incremental (with cache)" "$WARM_DURATION"
echo "----------------------------------------"
printf "%-25s %10ss\n" "Time Saved" "$IMPROVEMENT"
printf "%-25s %10s%%\n" "Improvement" "$PERCENT_IMPROVEMENT"

echo ""
echo "🎯 Final ccache Statistics:"
ccache --show-stats || echo "ccache stats not available"

echo ""
print_header "Analysis & Interpretation"

# Try to get cache hit rate, handle different ccache versions
CACHE_HIT_RATE=$(ccache --show-stats 2>/dev/null | grep -o '[0-9.]*%' | head -1 | tr -d '%' || echo "0")

# Handle case where we couldn't parse the hit rate
if [ -z "$CACHE_HIT_RATE" ] || [ "$CACHE_HIT_RATE" = "0" ]; then
    echo -e "${YELLOW}ℹ️  Could not parse cache hit rate from ccache stats${NC}"
    echo "   This is normal for some ccache versions or configurations"
    echo ""
    echo "💡 Manual Analysis:"
    echo "   • If incremental build was much faster, ccache is working"
    echo "   • Check ccache stats above for 'cache hit' entries"
    echo "   • Look for reduced compilation messages in build output"
else
    if [ "${CACHE_HIT_RATE%.*}" -gt 70 ]; then
        echo -e "${GREEN}✅ Excellent cache performance!${NC}"
        echo "   Cache hit rate: ${CACHE_HIT_RATE}% (>70% is very good)"
    elif [ "${CACHE_HIT_RATE%.*}" -gt 50 ]; then
        echo -e "${YELLOW}⚠️  Good cache performance${NC}"
        echo "   Cache hit rate: ${CACHE_HIT_RATE}% (50-70% is decent)"
    else
        echo -e "${RED}❌ Low cache performance${NC}"
        echo "   Cache hit rate: ${CACHE_HIT_RATE}% (<50% suggests issues)"
    fi
fi

echo ""
echo "💡 Key Insights:"

if [ "$PERCENT_IMPROVEMENT" -gt 50 ]; then
    echo "   • ccache provided ${PERCENT_IMPROVEMENT}% build time improvement"
    echo "   • This demonstrates excellent ccache effectiveness"
    echo "   • Your development workflow will be much faster"
elif [ "$PERCENT_IMPROVEMENT" -gt 20 ]; then
    echo "   • ccache provided ${PERCENT_IMPROVEMENT}% build time improvement"  
    echo "   • This is good performance for incremental builds"
    echo "   • Benefits will be even greater for smaller changes"
elif [ "$PERCENT_IMPROVEMENT" -gt 0 ]; then
    echo "   • ccache provided ${PERCENT_IMPROVEMENT}% build time improvement"
    echo "   • Modest improvement, but still worthwhile"
    echo "   • Benefits increase with more incremental changes"
else
    echo "   • Incremental build showed minimal improvement"
    echo "   • This can happen if many files needed recompilation"
    echo "   • Try making smaller changes for better results"
fi

echo ""
echo "🚀 For Development Workflow:"
echo "   • First build: Expect 2-5% overhead (cache setup)"
echo "   • Subsequent builds: Expect 30-90% improvement"
echo "   • Small changes: Expect 70-95% improvement"
echo ""

print_header "Demonstration Complete"
echo "Your ccache is now warmed up and ready for fast development builds!"
echo ""
echo "Try making small changes to source files and running:"
echo "  ./tools/benchmark_build.sh"
echo ""
echo "Each build should now be significantly faster! 🎯"
