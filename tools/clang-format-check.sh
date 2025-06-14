#!/bin/bash

# Check if clang-format is installed
if ! command -v clang-format &> /dev/null
then
  echo "Error: clang-format is not installed."
  exit 1
fi

# Find all C/C++ source files in src and include directories, excluding generated files
FILES=$(find src crates -type f \( -name "*.cpp" -o -name "*.cc" -o -name "*.h" -o -name "*.hpp" \) | grep -v "\.bin\.h$" | grep -v "\.autogen\.h$" | grep -v "\.autogen\.hpp$")

# Count total files for progress reporting
TOTAL=$(echo "$FILES" | wc -l | tr -d '[:space:]')
CURRENT=0
FAILED=0

echo "Starting format check for $TOTAL files..."

# Check formatting
for FILE in $FILES
do
  # Update progress counter
  ((CURRENT++))
  
  # Calculate percentage
  PERCENT=$((CURRENT * 100 / TOTAL))
  
  # Show progress
  printf "\r[%3d%%] Checking file %d/%d: %s" $PERCENT $CURRENT $TOTAL "$FILE"
  # Get formatting differences
  DIFF=$(clang-format --style=file "$FILE" | diff -u "$FILE" -)
  if [ -n "$DIFF" ]; then
    # Clear progress line
    echo -e "\n"
    echo "File $FILE is not formatted correctly:"
    echo "$DIFF"
    FAILED=1
  fi
done

# Clear progress line and show final result
echo -e "\n"

if [ $FAILED -eq 1 ]; then
  echo "❌ Formatting check failed. Run 'npm run format:fix' to fix formatting issues."
  exit 1
else
  echo "✅ All files are formatted correctly. ($TOTAL files checked)"
  exit 0
fi
