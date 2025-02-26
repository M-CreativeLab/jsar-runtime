# Set the common source files.
file(GLOB TR_COMMON_SOURCE
    "src/common/*.cpp"
    "src/common/**/*.cpp"
)

# Set the common include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/common
    ${CMAKE_SOURCE_DIR}/thirdparty/headers
)

# Set the common definitions for all targets
add_definitions(-DTR_LOGGING_SIMPLE_TAG=1)

# Set the common libraries to be linked by client, common and other tools
if (ANDROID)
    add_link_options(-Wl,--no-eh-frame-hdr) # Disable the generation of .eh_frame_hdr section
    link_libraries(log)
elseif (LINUX)
    link_libraries(X11 pthread)
elseif (WIN32)
    link_libraries(Winmm)
endif()

# Set the common cmake parameters
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build/output/release)
set(TR_RELEASE_DEST unknown)
if (APPLE)
    set(TR_RELEASE_DEST universal-apple-darwin)
elseif (ANDROID)
    set(TR_RELEASE_DEST aarch64-linux-android)
elseif (WIN32)
    set(TR_RELEASE_DEST x86_64-pc-windows-msvc)
endif()

# Set library linking parameters
set(STATICLIB_EXTNAME a)
set(DYNAMICLIB_EXTNAME so)
set(LIBRARY_PREFIX lib)
if (APPLE)
    set(DYNAMICLIB_EXTNAME dylib)
elseif (WIN32)
    set(STATICLIB_EXTNAME lib)
    set(DYNAMICLIB_EXTNAME dll)
    set(LIBRARY_PREFIX "")
endif()

# Set the Rust target
set(TR_CRATE_TARGET unknown)
if (APPLE)
    set(TR_CRATE_TARGET universal-apple-darwin)
elseif (ANDROID)
    set(TR_CRATE_TARGET aarch64-linux-android)
elseif (WIN32)
    set(TR_CRATE_TARGET x86_64-pc-windows-msvc)
endif()
message(STATUS "Transmute crates target name: ${TR_CRATE_TARGET}")

# Set the Rust build directory
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TR_CRATE_BUILD_DIR "debug")
else()
    set(TR_CRATE_BUILD_DIR "release")
endif()

# Set the thirdparty library path
if (APPLE)
    set(TR_THIRDPARTY_PATH ${CMAKE_SOURCE_DIR}/thirdparty/libs/${CMAKE_SYSTEM_NAME})
elseif (WIN32)
    set(TR_THIRDPARTY_PATH ${CMAKE_SOURCE_DIR}/thirdparty/libs/${CMAKE_SYSTEM_NAME}/x86_64)
else()
    set(TR_THIRDPARTY_PATH ${CMAKE_SOURCE_DIR}/thirdparty/libs/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR})
endif()
message(STATUS "Transmute thirdparty directory: ${TR_THIRDPARTY_PATH}")

# Set Node.js version used and its headers.
set(NODEJS_VERSION 18.12.1)
set(NODEJS_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/node-v${NODEJS_VERSION}/include)
set(NODEJS_LIBRARY_VERSION "")
if (APPLE)
    set(NODEJS_LIBRARY_VERSION ".108")
endif()
set(NODEJS_LIBRARY_BINARY_PATH ${TR_THIRDPARTY_PATH}/lib/${LIBRARY_PREFIX}node${NODEJS_LIBRARY_VERSION}.${DYNAMICLIB_EXTNAME})
include_directories(${NODEJS_HEADERS_PATH} ${NODEJS_HEADERS_PATH}/node)

# Skia headers
set(SKIA_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/skia)
include_directories(
    ${SKIA_HEADERS_PATH}
    ${SKIA_HEADERS_PATH}/include
)

# Add Unity headers
set(UNITY_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/Unity/include)
include_directories(${UNITY_HEADERS_PATH})

# Add LabSound headers
set(LABSOUND_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/LabSound/include)
include_directories(${LABSOUND_HEADERS_PATH})

# Add rust crates headers & libraries
include_directories(${CMAKE_SOURCE_DIR}/build/output/headers)

# Function to add library to the target
function(tr_target_link_library TARGET LIBRARY_DIRECTORY LIBRARY_NAME USE_DYNAMIC_LINK)
    if (USE_DYNAMIC_LINK STREQUAL DYNAMIC)
        target_link_libraries(${TARGET}
            PRIVATE ${LIBRARY_DIRECTORY}/${LIBRARY_PREFIX}${LIBRARY_NAME}.${DYNAMICLIB_EXTNAME})
    else()
        target_link_libraries(${TARGET}
            PRIVATE ${LIBRARY_DIRECTORY}/${LIBRARY_PREFIX}${LIBRARY_NAME}.${STATICLIB_EXTNAME})
    endif()
endfunction()

# Function to add thirdparty library to the target
function(tr_target_link_thirdparty_library TARGET LIBRARY_NAME)
    if (LIBRARY_NAME STREQUAL "node")
        tr_target_link_library(${TARGET} ${TR_THIRDPARTY_PATH}/lib ${LIBRARY_NAME} DYNAMIC)
    else()
        tr_target_link_library(${TARGET} ${TR_THIRDPARTY_PATH}/lib ${LIBRARY_NAME} STATIC)
    endif()
endfunction()

function(tr_target_link_skia_library TARGET)
    tr_target_link_thirdparty_library(${TARGET} skia)
    tr_target_link_thirdparty_library(${TARGET} skparagraph)
    tr_target_link_thirdparty_library(${TARGET} skshaper)
    tr_target_link_thirdparty_library(${TARGET} skunicode_core)
    tr_target_link_thirdparty_library(${TARGET} skunicode_icu)
endfunction()

function(tr_target_link_labsound_library TARGET LIBRARY_NAME)
    if (LIBRARY_NAME STREQUAL "node")
        tr_target_link_library(${TARGET} ${TR_THIRDPARTY_PATH}/lib/LabSound ${LIBRARY_NAME} DYNAMIC)
    else()
        tr_target_link_library(${TARGET} ${TR_THIRDPARTY_PATH}/lib/LabSound ${LIBRARY_NAME} STATIC)
    endif()
endfunction()

# Function to install the target to the release directory
function(tr_target_install TARGET)
    install(TARGETS ${TARGET} DESTINATION ${TR_RELEASE_DEST})
endfunction()

# Function to install the thirdparty library to the release directory
function(tr_install_nodejs)
    if (APPLE)
        install(FILES ${TR_THIRDPARTY_PATH}/lib/libnode.dylib DESTINATION ${TR_RELEASE_DEST})
        install(FILES ${TR_THIRDPARTY_PATH}/lib/libnode.108.dylib DESTINATION ${TR_RELEASE_DEST})
    elseif (ANDROID)
        install(FILES ${TR_THIRDPARTY_PATH}/lib/libnode.so DESTINATION ${TR_RELEASE_DEST})
    elseif (WIN32)
        install(FILES ${TR_THIRDPARTY_PATH}/lib/libnode.dll DESTINATION ${TR_RELEASE_DEST})
    endif()
endfunction()

# Function to set properties
function(tr_target_set_properties TARGET)
    if (APPLE)
        cmake_parse_arguments(APPLE_ARG "USE_EXECUTABLE_PATH" "" "" ${ARGN})
        if (APPLE_ARG_USE_EXECUTABLE_PATH)
            set(APPLE_TARGET_SUFFIX "")
            set(APPLE_TARGET_INSTALL_RPATH "@executable_path")
        else()
            set(APPLE_TARGET_SUFFIX ".dylib")
            set(APPLE_TARGET_INSTALL_RPATH "@loader_path")
        endif()
        set_target_properties(${TARGET} PROPERTIES
            SUFFIX "${APPLE_TARGET_SUFFIX}"
            INSTALL_RPATH "${APPLE_TARGET_INSTALL_RPATH}"
            BUILD_WITH_INSTALL_RPATH TRUE
        )
    elseif (ANDROID)
        cmake_parse_arguments(ANDROID_ARG "USE_EXECUTABLE_PATH" "" "" ${ARGN})
        if (ANDROID_ARG_USE_EXECUTABLE_PATH)
            set(ANDROID_TARGET_SUFFIX "")
        else()
            set(ANDROID_TARGET_SUFFIX ".so")
        endif()
        target_link_options(${TARGET} PRIVATE -Wl,-rpath,$ORIGIN)
        set_target_properties(${TARGET} PROPERTIES
            SUFFIX "${ANDROID_TARGET_SUFFIX}"
        )
    elseif (WIN32)
        set_target_properties(${TARGET} PROPERTIES
            SUFFIX ".dll"
        )
    endif ()
endfunction()
