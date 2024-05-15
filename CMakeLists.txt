cmake_minimum_required(VERSION 3.6)

project(transmute)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILECOMMANDS ON)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_SKIP_INSTALL_RPATH FALSE)

set(TR_VERSION_MAJOR 1)
set(TR_VERSION_MINOR 0)
set(TR_VERSION_PATCH 0)

execute_process(
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND git rev-parse --short HEAD
  RESULT_VARIABLE SHORT_HASH_RESULT
  OUTPUT_VARIABLE TR_COMMIT_SHORT_HASH)
execute_process(
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND git rev-parse HEAD
  RESULT_VARIABLE SHORT_HASH_RESULT
  OUTPUT_VARIABLE TR_COMMIT_HASH)
string(STRIP ${TR_COMMIT_HASH} TR_COMMIT_HASH)
string(STRIP ${TR_COMMIT_SHORT_HASH} TR_COMMIT_SHORT_HASH)

message("Transmute Version ${TR_VERSION_MAJOR}.${TR_VERSION_MINOR}.${TR_VERSION_PATCH}")
message("          Commit ${TR_COMMIT_HASH}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if (WIN32)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
else ()
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# This compiles all the libraries with -fPIC, which is critical to link a static
# library into a shared lib.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(USE_LLD "Use lld (from llvm) linker" OFF)
option(USE_MOLD "Use mold (A Modern Linker)" OFF)

if(LINUX OR APPLE)
    if (NOT IOS)
        # (penguinliong) Not compatible with -fembed-bitcode. Not sure where it
        # comes from; probably a XCode preset?
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
    endif()
endif()

if (USE_LLD)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=lld")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")

    if (WIN32)
        if (CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
            # -debug for LLD generates PDB files
            set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-debug")
            set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-debug")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-debug")
        endif()
    endif()
endif()

if (USE_MOLD)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=mold")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fuse-ld=mold")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=mold")
endif()

if (WIN32)
  # For `Debug` configs MSVC links to a debuggable runtime by default which has
  # symbol conflicts with the prebuilt LLVM in `Release`. We should be providing
  # prebuilt LLVMs for both `Debug` and `Release` but LLVM 10 cannot be built by
  # MSVC in `Debug` config because MSVC would try to fill uninitialize memory
  # with `0xCC` but it too breaks `LLVMTableGen` which is depended on by almost
  # every component in LLVM.
  message("CMAKE_MSVC_RUNTIME_LIBRARY: ${CMAKE_MSVC_RUNTIME_LIBRARY}")
endif()

if (APPLE)
    # Build Universal Binary for Apple Silicon that works for intel and silicon.
    # set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    message("CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")
endif()

# Enable LTO
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

include(cmake/TransmuteCXXFlags.cmake)
include(cmake/TransmuteCore.cmake)

message("C++ Flags: ${CMAKE_CXX_FLAGS}")
message("Build type: ${CMAKE_BUILD_TYPE}")
