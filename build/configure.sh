#!/bin/bash

# Global defaults
TARGET=""
BUILD_TYPE="Release"
BUILD_DIR=""
SOURCE_DIR=".."
VERBOSE=0
ENABLE_INSPECTOR=0

# Android-specific defaults
ANDROID_ABI="arm64-v8a"
ANDROID_STL="c++_static"
ANDROID_PLATFORM="android-26"
PRINT_TOOLCHAIN_SETUP=0

# Darwin-specific defaults
GENERATOR="Unix Makefiles"
CXX_COMPILER="clang++"
OSX_DEPLOYMENT_TARGET="11"

function show_help() {
  echo "Usage: $0 <TARGET> [options]"
  echo "Targets:"
  echo "  android             Build for Android"
  echo "  darwin              Build for macOS"
  echo ""
  echo "Global options:"
  echo "  --build-dir <DIR>    Build directory (default: targets/<TARGET>)"
  echo "  --build-inspector    Enable TR_ENABLE_INSPECTOR flag"
  echo "  --source-dir <DIR>   Source directory (default: ..)"
  echo "  --debug              Debug build"
  echo "  --release            Release build (default)"
  echo "  --verbose            Verbose output"
  echo "  --help               Show this help"
  echo ""
  echo "Android options:"
  echo "  --abi <ABI>          Android ABI (default: $ANDROID_ABI)"
  echo "  --stl <STL>          Android STL (default: $ANDROID_STL)"
  echo "  --platform <VER>     Android platform (default: $ANDROID_PLATFORM)"
  echo "  --print-toolchain    Print toolchain setup command"
  echo ""
  echo "macOS options:"
  echo "  --generator <GEN>    CMake generator (default: $GENERATOR)"
  echo "  --compiler <CXX>     C++ compiler (default: $CXX_COMPILER)"
  echo "  --osx-target <VER>   macOS deployment target (default: $OSX_DEPLOYMENT_TARGET)"
  exit 0
}

# Validate first argument as target
if [[ $# -eq 0 ]]; then
  echo "Error: Missing target parameter"
  show_help
  exit 1
fi

TARGET=$1
shift # Remove target from arguments

# Validate target
case $TARGET in
android | darwin) ;;
*)
  echo "Invalid target: $TARGET"
  echo "Valid targets: android, darwin"
  exit 1
  ;;
esac

# Parse remaining arguments
while [[ $# -gt 0 ]]; do
  case $1 in
  # Global options
  --build-dir)
    BUILD_DIR="$2"
    shift 2
    ;;
  --build-inspector)
    ENABLE_INSPECTOR=1
    shift
    ;;
  --source-dir)
    SOURCE_DIR="$2"
    shift 2
    ;;
  --debug)
    BUILD_TYPE="Debug"
    shift
    ;;
  --release)
    BUILD_TYPE="Release"
    shift
    ;;
  --verbose)
    VERBOSE=1
    shift
    ;;
  --help)
    show_help
    ;;

  # Android options
  --abi)
    [[ $TARGET == "android" ]] || {
      echo "--abi is Android-only option"
      exit 1
    }
    ANDROID_ABI="$2"
    shift 2
    ;;
  --stl)
    [[ $TARGET == "android" ]] || {
      echo "--stl is Android-only option"
      exit 1
    }
    ANDROID_STL="$2"
    shift 2
    ;;
  --platform)
    [[ $TARGET == "android" ]] || {
      echo "--platform is Android-only option"
      exit 1
    }
    ANDROID_PLATFORM="$2"
    shift 2
    ;;
  --print-toolchain)
    [[ $TARGET == "android" ]] || {
      echo "--print-toolchain is Android-only option"
      exit 1
    }
    PRINT_TOOLCHAIN_SETUP=1
    shift
    ;;

  # Darwin options
  --generator)
    [[ $TARGET == "darwin" ]] || {
      echo "--generator is macOS-only option"
      exit 1
    }
    GENERATOR="$2"
    shift 2
    ;;
  --compiler)
    [[ $TARGET == "darwin" ]] || {
      echo "--compiler is macOS-only option"
      exit 1
    }
    CXX_COMPILER="$2"
    shift 2
    ;;
  --osx-target)
    [[ $TARGET == "darwin" ]] || {
      echo "--osx-target is macOS-only option"
      exit 1
    }
    OSX_DEPLOYMENT_TARGET="$2"
    shift 2
    ;;

  *)
    echo "Unknown option: $1"
    exit 1
    ;;
  esac
done

# Set default build directory
if [[ -z $BUILD_DIR ]]; then
  BUILD_DIR="targets/$TARGET"
fi

# Platform-specific validation
if [[ $TARGET == "android" ]]; then
  # Android environment setup
  if [[ -z "$ANDROID_NDK_HOME" ]]; then
    echo "Error: ANDROID_NDK_HOME is not set"
    exit 1
  fi

  # Detect host parameters
  HOST_OS=$(uname -s | tr '[:upper:]' '[:lower:]')
  [[ $HOST_OS == "darwin" ]] && HOST_OS="darwin" || HOST_OS="linux"
  HOST_ARCH="x86_64"

  # Toolchain setup
  TOOLCHAIN="$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/$HOST_OS-$HOST_ARCH"
  if [[ ! -d "$TOOLCHAIN" ]]; then
    echo "Error: Android toolchain not found at: $TOOLCHAIN"
    exit 1
  fi
  export PATH="$TOOLCHAIN/bin:$PATH"

  # Handle print-toolchain request
  if [[ $PRINT_TOOLCHAIN_SETUP -eq 1 ]]; then
    echo "export PATH=$TOOLCHAIN/bin:\$PATH"
    exit 0
  fi

elif [[ $TARGET == "darwin" ]]; then
  # Verify CMake installation
  if ! command -v cmake &>/dev/null; then
    echo "Error: CMake is required but not installed"
    exit 1
  fi
fi

# Verbose output
if [[ $VERBOSE -eq 1 ]]; then
  echo "=== Build Configuration ==="
  echo "Target Platform:      $TARGET"
  echo "Build Type:          $BUILD_TYPE"
  echo "Build Directory:     $BUILD_DIR"
  echo "Source Directory:    $SOURCE_DIR"
  echo "Enable Inspector:    $ENABLE_INSPECTOR"

  if [[ $TARGET == "android" ]]; then
    echo "Android ABI:         $ANDROID_ABI"
    echo "Android STL:         $ANDROID_STL"
    echo "Android Platform:    $ANDROID_PLATFORM"
    echo "NDK Toolchain:      $TOOLCHAIN"
  elif [[ $TARGET == "darwin" ]]; then
    echo "CMake Generator:     $GENERATOR"
    echo "C++ Compiler:       $CXX_COMPILER"
    echo "macOS Target:       $OSX_DEPLOYMENT_TARGET"
  fi
  echo "==========================="
fi

# Build CMake command
CMAKE_CMD=(
  cmake
  -S "$SOURCE_DIR"
  -B "$BUILD_DIR"
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
)

# Add platform-specific parameters
if [[ $TARGET == "android" ]]; then
  CMAKE_CMD+=(
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake"
    -DANDROID_ABI="$ANDROID_ABI"
    -DANDROID_STL="$ANDROID_STL"
    -DANDROID_PLATFORM="$ANDROID_PLATFORM"
  )
elif [[ $TARGET == "darwin" ]]; then
  CMAKE_CMD+=(
    -G "$GENERATOR"
    -DCMAKE_CXX_COMPILER="$CXX_COMPILER"
    -DCMAKE_OSX_DEPLOYMENT_TARGET="$OSX_DEPLOYMENT_TARGET"
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  )
fi

# Add inspector flag
if [[ $ENABLE_INSPECTOR -eq 1 ]]; then
  CMAKE_CMD+=(-DTR_ENABLE_INSPECTOR=ON)
fi

# Execute CMake
echo "Generating build system..."
"${CMAKE_CMD[@]}"

if [[ $? -eq 0 ]]; then
  echo "Build system generated at: $BUILD_DIR"
else
  echo "Failed to generate build system"
  exit 1
fi
