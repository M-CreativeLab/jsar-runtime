# Install script for directory: /home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/runner/work/jsar-runtime/jsar-runtime/build/output/release")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/runner/work/jsar-runtime/jsar-runtime/build/ggml/src/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/jsar-runtime/jsar-runtime/build/ggml/src/libggml.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-cpu.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-alloc.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-backend.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-blas.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-cann.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-cpp.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-cuda.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-opt.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-metal.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-rpc.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-sycl.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-vulkan.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/ggml-webgpu.h"
    "/home/runner/work/jsar-runtime/jsar-runtime/thirdparty/ggml/include/gguf.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/runner/work/jsar-runtime/jsar-runtime/build/ggml/src/libggml-base.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ggml" TYPE FILE FILES
    "/home/runner/work/jsar-runtime/jsar-runtime/build/ggml/ggml-config.cmake"
    "/home/runner/work/jsar-runtime/jsar-runtime/build/ggml/ggml-version.cmake"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/jsar-runtime/jsar-runtime/build/ggml/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
