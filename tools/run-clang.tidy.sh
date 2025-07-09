#!/bin/bash

set -e

cd "$(dirname "$0")/.."  # Go to project root

LOG_DIR="tools/logs"
LOG_FILE="$LOG_DIR/clang-tidy.log"

mkdir -p "$LOG_DIR"
echo "[clang-tidy] Static analysis started at $(date)" > "$LOG_FILE"

# Step 1: Generate compile_commands.json if missing
if [ ! -f build/compile_commands.json ]; then
  echo "[clang-tidy] Generating compile_commands.json..."
  cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
fi

# Step 2: Get system include paths as --extra-arg flags
STD_INCLUDES=$(echo | g++ -E -x c++ - -v 2>&1 | \
  awk '/#include <...> search starts here:/{flag=1;next}/End of search list/{flag=0}flag' | \
  sed 's/^ /--extra-arg=-isystem/' | paste -sd ' ' -)

# Step 3: Find .cpp files in modules/ and apps/ only (excluding external/)
CPP_FILES=$(find modules apps -type f -name '*.cpp')

# Step 4: Run clang-tidy on each file with logging
for file in $CPP_FILES; do
  echo "[clang-tidy] Checking $file ..."
  echo -e "\n\n[FILE] $file" >> "$LOG_FILE"
  clang-tidy "$file" -p build --header-filter='^(modules|apps)/' $STD_INCLUDES "$@" >> "$LOG_FILE" 2>&1
done

echo "[clang-tidy] Static analysis completed at $(date)"
echo "[clang-tidy] Log saved to $LOG_FILE"
