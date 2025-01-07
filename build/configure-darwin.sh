#!/bin/bash

# Default values
BUILD_TYPE="Release"
GENERATOR="Unix Makefiles"
CXX_COMPILER="clang++"
OSX_DEPLOYMENT_TARGET="11"
BUILD_DIR="targets/darwin"
VERBOSE=0

# Function to display help
function show_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  --debug           Set build type to Debug"
    echo "  --release         Set build type to Release (default)"
    echo "  --generator <G>   Set CMake generator (default: Unix Makefiles)"
    echo "  --compiler <C>    Set C++ compiler (default: clang++)"
    echo "  --target <T>      Set OSX deployment target (default: 11)"
    echo "  --build-dir <D>   Set build directory (default: targets/darwin)"
    echo "  --verbose         Enable verbose output"
    echo "  --help            Show this help message"
    exit 0
}

# Check if CMake is installed
if ! command -v cmake &>/dev/null; then
    echo "CMake could not be found. Please install CMake and try again."
    exit 1
fi

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
    --debug)
        BUILD_TYPE="Debug"
        shift
        ;;
    --release)
        BUILD_TYPE="Release"
        shift
        ;;
    --generator)
        GENERATOR="$2"
        shift 2
        ;;
    --compiler)
        CXX_COMPILER="$2"
        shift 2
        ;;
    --target)
        OSX_DEPLOYMENT_TARGET="$2"
        shift 2
        ;;
    --build-dir)
        BUILD_DIR="$2"
        shift 2
        ;;
    --verbose)
        VERBOSE=1
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
    echo "Build Type: $BUILD_TYPE"
    echo "Generator: $GENERATOR"
    echo "C++ Compiler: $CXX_COMPILER"
    echo "OSX Deployment Target: $OSX_DEPLOYMENT_TARGET"
    echo "Build Directory: $BUILD_DIR"
fi

# Run CMake command
cmake \
    -G "$GENERATOR" \
    -DCMAKE_CXX_COMPILER="$CXX_COMPILER" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="$OSX_DEPLOYMENT_TARGET" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -B "$BUILD_DIR" \
    ..

# Check if CMake command was successful
if [ $? -eq 0 ]; then
    echo "CMake configuration successful."
else
    echo "CMake configuration failed."
    exit 1
fi
