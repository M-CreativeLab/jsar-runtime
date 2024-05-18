file(GLOB TR_CLIENT_SOURCE
    "src/bindings/canvas/*.cpp"
    "src/bindings/env/*.cpp"
    "src/bindings/logger/*.cpp"
    "src/bindings/messaging/*.cpp"
    "src/bindings/renderer/*.cpp"
    "src/bindings/webgl/*.cpp"
    "src/bindings/webxr/*.cpp"
    "src/client/*.cpp"
)
add_executable(TransmuteClient
    ${TR_COMMON_SOURCE}
    ${TR_CLIENT_SOURCE}
)

set(TR_CRATE_TARGET unknown)
if (APPLE)
    set(TR_CRATE_TARGET universal-apple-darwin)
elseif (ANDROID)
    set(TR_CRATE_TARGET aarch64-linux-android)
elseif (WIN32)
    set(TR_CRATE_TARGET x86_64-pc-windows-msvc)
endif()
message(STATUS "Transmute crates target name: ${TR_CRATE_TARGET}")

set(NODE_ADDON_API_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/node-addon-api/include)
target_include_directories(TransmuteClient PRIVATE ${NODE_ADDON_API_HEADERS_PATH})

# Link libraries
tr_target_link_library(TransmuteClient ${CMAKE_SOURCE_DIR}/build/output/crates/${TR_CRATE_TARGET}/release jsar_jsbindings STATIC)
tr_target_link_thirdparty_library(TransmuteClient node)
if (ANDROID)
    tr_target_link_thirdparty_library(TransmuteClient skia) # TODO: support skia for other platforms?
endif()

# Set the rpath
if (ANDROID)
    target_link_options(TransmuteClient PRIVATE -Wl,-rpath,$ORIGIN)
endif()

# Install client
tr_target_install(TransmuteClient)

# Install Node.js
tr_install_nodejs()
