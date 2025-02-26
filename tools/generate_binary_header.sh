#!/bin/bash

# Function to display usage information
usage() {
  echo "Usage: $0 NAME INPUT_FILE OUTPUT_HEADER [--platform=PLATFORM]"
  echo ""
  echo "Arguments:"
  echo "  NAME          The name of the binary data (used as a prefix in the header file)."
  echo "  INPUT_FILE    The input binary file to be converted."
  echo "  OUTPUT_HEADER The output header file to be generated."
  echo ""
  echo "Options:"
  echo "  --platform=PLATFORM  Specify the platform (e.g., android)."
  echo "                        If set to 'android', ANDROID_NDK_HOME must be defined."
  exit 1
}

# Initialize variables
NAME=""
INPUT_FILE=""
OUTPUT_HEADER=""
PLATFORM=""

# Parse command-line arguments using getopts
while [[ $# -gt 0 ]]; do
  case "$1" in
    --platform=*)
      PLATFORM="${1#*=}"
      shift
      ;;
    *)
      if [[ -z "$NAME" ]]; then
        NAME="$1"
      elif [[ -z "$INPUT_FILE" ]]; then
        INPUT_FILE="$1"
      elif [[ -z "$OUTPUT_HEADER" ]]; then
        OUTPUT_HEADER="$1"
      else
        echo "Error: Too many arguments."
        usage
      fi
      shift
      ;;
  esac
done

# Validate required arguments
if [[ -z "$NAME" || -z "$INPUT_FILE" || -z "$OUTPUT_HEADER" ]]; then
  echo "Error: Missing required arguments."
  usage
fi

# Check if xxd command is available
if ! command -v xxd >/dev/null; then
  echo "Error: xxd command not found. Please install it and try again."
  exit 1
fi

# Handle platform-specific logic
if [[ "$PLATFORM" == "android" ]]; then
  if [[ -z "$ANDROID_NDK_HOME" ]]; then
    echo "Error: ANDROID_NDK_HOME is not set. Please set the ANDROID_NDK_HOME environment variable."
    exit 1
  fi
  # Strip the input file before generating the header
  STRIP_TOOL="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-strip"
  if [[ ! -x "$STRIP_TOOL" ]]; then
    echo "Error: llvm-strip not found at $STRIP_TOOL. Please check your ANDROID_NDK_HOME setup."
    exit 1
  fi
  "$STRIP_TOOL" "$INPUT_FILE" || { echo "Error: Failed to strip the input file."; exit 1; }
fi

# Generate binary header with xxd
gzip -c "$INPUT_FILE" | xxd -n "$NAME" -i - >"$OUTPUT_HEADER" || { echo "Error: Failed to generate the header file."; exit 1; }

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
echo "const char* ${NAME//[^a-zA-Z0-9]/_}_md5 = \"$MD5\";" >>"$OUTPUT_HEADER" || { echo "Error: Failed to append MD5 checksum."; exit 1; }

echo "Header file generated successfully: $OUTPUT_HEADER"
