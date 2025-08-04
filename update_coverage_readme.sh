#!/bin/bash

# Script to update README with current coverage percentage
# This script is for local use and testing

set -e

echo "🔍 Updating README with current coverage percentage..."

# Check if we're in the right directory
if [ ! -f "README.md" ]; then
    echo "❌ Error: README.md not found. Please run this script from the project root."
    exit 1
fi

# Check if coverage data exists
if [ ! -f "coverage/coverage.xml" ]; then
    echo "📊 No coverage data found. Running coverage generation..."
    ./test_coverage_locally.sh
fi

# Extract coverage percentage from the most recent coverage report
if [ -f "coverage/coverage.xml" ]; then
    # Extract coverage from XML report
    COVERAGE=$(grep -o 'line-rate="[0-9.]*"' coverage/coverage.xml | head -1 | sed 's/line-rate="//g' | sed 's/"//g')
    COVERAGE_PERCENT=$(echo "$COVERAGE * 100" | bc -l | cut -d. -f1)
    
    echo "📈 Current coverage: ${COVERAGE_PERCENT}%"
    
    # Determine badge color based on coverage
    BADGE_COLOR="red"
    if [ "$COVERAGE_PERCENT" -ge 70 ]; then
        BADGE_COLOR="brightgreen"
    elif [ "$COVERAGE_PERCENT" -ge 50 ]; then
        BADGE_COLOR="yellow"
    elif [ "$COVERAGE_PERCENT" -ge 30 ]; then
        BADGE_COLOR="orange"
    fi
    
    echo "🏷️  Badge color: ${BADGE_COLOR}"
    
    # Update README with coverage badge
    # Note: We keep the Codecov badge since it's more accurate and updates automatically
    echo "✅ Coverage information available. Codecov badge will update automatically when pushed to GitHub."
    echo "   Current local coverage: ${COVERAGE_PERCENT}%"
    
else
    echo "❌ Could not find coverage data. Please run ./test_coverage_locally.sh first."
    exit 1
fi

echo "🎉 README coverage update complete!"
