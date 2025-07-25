#!/bin/bash

# ADSIL Simulation - Integration script for unused features detection

set -e

cd "$(dirname "$0")/.."  # Go to project root

echo "🔍 Running Unused Features Detection..."

# Run the detection tool
python3 tools/detect_unused_features.py

# Check the exit code
if [ $? -eq 0 ]; then
    echo "✅ No unused features found - all modules are properly configured!"
    exit 0
else
    echo "⚠️  Found unused features or configuration issues. Check the report above."
    echo "📄 Detailed report saved to tools/unused_features_report.json"
    exit 1
fi