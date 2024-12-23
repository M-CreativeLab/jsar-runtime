file(GLOB TR_CLIENT_SOURCE
    "src/bindings/browser/*.cpp"
    "src/bindings/canvas/*.cpp"
    "src/bindings/dom/*.cpp"
    "src/bindings/dom/**/*.cpp"
    "src/bindings/env/*.cpp"
    "src/bindings/logger/*.cpp"
    "src/bindings/math3d/*.cpp"
    "src/bindings/messaging/*.cpp"
    "src/bindings/renderer/*.cpp"
    "src/bindings/webgl/*.cpp"
    "src/bindings/webxr/*.cpp"
    "src/client/*.cpp"
    "src/client/**/*.cpp"
    "src/pugixml/*.cpp"
)

# Add the client shaders header target
set(TR_SHADERS_SOURCE "${CMAKE_SOURCE_DIR}/src/client/builtin_scene/")
set(TR_SHADERS_HEADER "${CMAKE_SOURCE_DIR}/src/client/builtin_scene/shaders_store.gen.hpp")
set(TR_SHADERS_HEADER_GENERATOR "${CMAKE_SOURCE_DIR}/tools/generate_shaders_header.cmake")
add_custom_target(TransmuteClientShadersHeader
    ALL
    COMMAND ${CMAKE_COMMAND} -DHEADER_OUTPUT=${TR_SHADERS_HEADER} -DSHADERS_DIR=${TR_SHADERS_SOURCE} -P ${TR_SHADERS_HEADER_GENERATOR}
    COMMENT "[target] Generating the shaders header"
)

# Add executable target
add_executable(TransmuteClient
    ${TR_COMMON_SOURCE}
    ${TR_CLIENT_SOURCE}
)
add_dependencies(TransmuteClient TransmuteClientShadersHeader)

set(NODE_ADDON_API_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/node-addon-api/include)
target_include_directories(TransmuteClient PRIVATE ${NODE_ADDON_API_HEADERS_PATH})

# Link libraries
tr_target_link_library(TransmuteClient ${CMAKE_SOURCE_DIR}/build/output/crates/${TR_CRATE_TARGET}/release jsar_jsbindings STATIC)
tr_target_link_thirdparty_library(TransmuteClient node)
tr_target_link_thirdparty_library(TransmuteClient skia)

if(APPLE)
    target_link_libraries(TransmuteClient PRIVATE "-framework CoreFoundation")
    target_link_libraries(TransmuteClient PRIVATE "-framework Carbon")
endif()

# Set properties
tr_target_set_properties(TransmuteClient USE_EXECUTABLE_PATH True)

# Install client
tr_target_install(TransmuteClient)

# Install Node.js
tr_install_nodejs()
