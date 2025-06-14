#!/bin/bash

# Format fix script
# This script finds all C/C++ files and applies clang-format to them

# Find all C/C++ source files, excluding generated files
FILES=$(find src crates -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.h" -o -name "*.hpp" \) | grep -v "\.bin\.h$" | grep -v "\.autogen\.h$" | grep -v "\.autogen\.hpp$")

# Set up counters for progress reporting
TOTAL=$(echo "$FILES" | wc -l | tr -d '[:space:]')
CURRENT=0

echo "Starting code formatting for $TOTAL files..."

# Process each file
for FILE in $FILES; do
  # Update progress counter
  ((CURRENT++))
  
  # Calculate percentage
  PERCENT=$((CURRENT * 100 / TOTAL))
  
  # Show progress
  printf "\r[%3d%%] Formatting file %d/%d: %s" $PERCENT $CURRENT $TOTAL "$FILE"
  
  # Format the file
  clang-format -i "$FILE"
done

# Clear progress line and show final result
echo -e "\n"
echo "✅ Formatting complete! Processed $TOTAL files."
exit 0
