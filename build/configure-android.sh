#!/bin/bash

# Default values
HOST_OS=""
HOST_ARCH="x86_64"
ANDROID_ABI="arm64-v8a"
ANDROID_STL="c++_static"
ANDROID_PLATFORM="android-26"
BUILD_TYPE="Release"
BUILD_DIR="targets/android"
SOURCE_DIR=".."
VERBOSE=0
PRINT_TOOLCHAIN_SETUP=0

# Function to display help
function show_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  --abi <ABI>          Set Android ABI (default: arm64-v8a)"
    echo "  --stl <STL>          Set Android STL (default: c++_static)"
    echo "  --platform <PLATFORM> Set Android platform (default: android-26)"
    echo "  --release            Set build type to Release (default)"
    echo "  --debug              Set build type to Debug"
    echo "  --build-dir <DIR>    Set build directory (default: targets/android)"
    echo "  --source-dir <DIR>   Set source directory (default: ..)"
    echo "  --verbose            Enable verbose output"
    echo "  --print-toolchain-setup  Print the Android Toolchain path setup command for other projects"
    echo "  --help               Show this help message"
    exit 0
}

# Check if ANDROID_NDK_HOME is set
if [ -z "$ANDROID_NDK_HOME" ]; then
    echo "Error: ANDROID_NDK_HOME is not set. Please set the ANDROID_NDK_HOME environment variable."
    exit 1
fi

# Detect host OS
if [ "$(uname -s)" == "Darwin" ]; then
    HOST_OS="darwin"
else
    HOST_OS="linux"
fi

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
    --abi)
        ANDROID_ABI="$2"
        shift 2
        ;;
    --stl)
        ANDROID_STL="$2"
        shift 2
        ;;
    --platform)
        ANDROID_PLATFORM="$2"
        shift 2
        ;;
    --release)
        BUILD_TYPE="Release"
        shift
        ;;
    --debug)
        BUILD_TYPE="Debug"
        shift
        ;;
    --build-dir)
        BUILD_DIR="$2"
        shift 2
        ;;
    --source-dir)
        SOURCE_DIR="$2"
        shift 2
        ;;
    --verbose)
        VERBOSE=1
        shift
        ;;
    --print-toolchain-setup)
        PRINT_TOOLCHAIN_SETUP=1
        shift
        ;;
    --help)
        show_help
        ;;
    *)
        echo "Unknown option: $1"
        exit 1
        ;;
    esac
done

# Verbose output
if [ $VERBOSE -eq 1 ]; then
    echo "Host OS: $HOST_OS"
    echo "Host Architecture: $HOST_ARCH"
    echo "Android ABI: $ANDROID_ABI"
    echo "Android STL: $ANDROID_STL"
    echo "Android Platform: $ANDROID_PLATFORM"
    echo "Build Type: $BUILD_TYPE"
    echo "Build Directory: $BUILD_DIR"
    echo "Source Directory: $SOURCE_DIR"
fi

# Set up toolchain and environment
TOOLCHAIN=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/$HOST_OS-$HOST_ARCH

# Check if the toolchain directory exists
if [ ! -d "$TOOLCHAIN" ]; then
    echo "Error: Toolchain directory does not exist: $TOOLCHAIN"
    echo "Please ensure ANDROID_NDK_HOME is set correctly and the toolchain is installed."
    exit 1
fi

export PATH=$TOOLCHAIN/bin:$PATH

if [ $VERBOSE -eq 1 ]; then
    echo "Toolchain Path: $TOOLCHAIN"
    echo "Updated PATH: $PATH"
fi

# If --print-toolchain-setup is provided, print the export command and exit
if [ $PRINT_TOOLCHAIN_SETUP -eq 1 ]; then
    echo "export PATH=$TOOLCHAIN/bin:\$PATH"
    exit 0
fi

# Run CMake command
cmake \
    -G "Unix Makefiles" \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=$ANDROID_ABI \
    -DANDROID_STL=$ANDROID_STL \
    -DANDROID_PLATFORM=$ANDROID_PLATFORM \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -B $BUILD_DIR \
    $SOURCE_DIR

# Check if CMake command was successful
if [ $? -eq 0 ]; then
    echo "CMake configuration successful."
else
    echo "CMake configuration failed."
    exit 1
fi
