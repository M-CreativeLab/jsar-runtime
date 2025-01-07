#!/bin/bash

# Name
NAME=$1
# Input binary file
INPUT_FILE=$2
# Output header file
OUTPUT_HEADER=$3

# Check if xxd command is available
if ! command -v xxd >/dev/null; then
  echo "Error: xxd command not found. Please install it and try again."
  exit 1
fi

# Generate binary header with xxd
gzip -c "$INPUT_FILE" | xxd -n "$NAME" -i - >"$OUTPUT_HEADER"

# Detect the operating system and compute MD5 checksum
if command -v md5sum >/dev/null; then
  # Linux or systems with md5sum
  MD5=$(md5sum "$INPUT_FILE" | awk '{print $1}')
elif command -v md5 >/dev/null; then
  # macOS or systems with md5
  MD5=$(md5 -q "$INPUT_FILE")
else
  echo "Error: Neither md5sum nor md5 command is available."
  exit 1
fi

# Append MD5 checksum to the header
echo "" >>"$OUTPUT_HEADER"
echo "// MD5 checksum of the file" >>"$OUTPUT_HEADER"
echo "const char* ${NAME//[^a-zA-Z0-9]/_}_md5 = \"$MD5\";" >>"$OUTPUT_HEADER"
