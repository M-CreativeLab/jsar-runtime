file(GLOB_RECURSE TR_CLIENT_LIBRARY_SOURCE
    "src/bindings/*.cpp"
    "src/client/animation/*.cpp"
    "src/client/browser/*.cpp"
    "src/client/builtin_scene/*.cpp"
    "src/client/canvas/*.cpp"
    "src/client/cssom/*.cpp"
    "src/client/dom/*.cpp"
    "src/client/frame/*.cpp"
    "src/client/graphics/*.cpp"
    "src/client/html/*.cpp"
    "src/client/layout/*.cpp"
    "src/client/media/*.cpp"
    "src/client/networking/*.cpp"
    "src/client/script_bindings/*.cpp"
    "src/client/scripting_base/*.cpp"
    "src/client/scroll/*.cpp"
    "src/client/workers/*.cpp"
    "src/client/xr/*.cpp"
    "src/client/logger.cpp"
    "src/client/per_process.cpp"
    "src/pugixml/*.cpp"
)
file(GLOB TR_CLIENT_SOURCE
    "src/client/hive_server.cpp"
    "src/client/entry.cpp"
    "src/client/main.cpp"
)

if(TR_ENABLE_INSPECTOR)
    file(GLOB_RECURSE TR_CLIENT_INSPECTOR_SOURCE
        "src/client/inspector/*.cpp"
    )
    list(APPEND TR_CLIENT_LIBRARY_SOURCE ${TR_CLIENT_INSPECTOR_SOURCE})
endif()

# Add the client shaders header target
set(TR_SHADERS_SOURCE "${CMAKE_SOURCE_DIR}/src/client/builtin_scene/")
set(TR_SHADERS_HEADER "${CMAKE_SOURCE_DIR}/src/client/builtin_scene/shaders_store.gen.hpp")
set(TR_SHADERS_HEADER_GENERATOR "${CMAKE_SOURCE_DIR}/tools/generate_shaders_header.cmake")
file(GLOB_RECURSE TR_SHADERS_SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/client/builtin_scene/*.vert"
    "${CMAKE_SOURCE_DIR}/src/client/builtin_scene/*.frag"
)
message(STATUS "Shaders source files: ${TR_SHADERS_SOURCE_FILES}")
add_custom_command(
    OUTPUT ${TR_SHADERS_HEADER}
    COMMAND ${CMAKE_COMMAND} -DHEADER_OUTPUT=${TR_SHADERS_HEADER} -DSHADERS_DIR=${TR_SHADERS_SOURCE} -P ${TR_SHADERS_HEADER_GENERATOR}
    COMMENT "[target] Generating the shaders header"
    DEPENDS ${TR_SHADERS_SOURCE_FILES}
    COMMENT "Generating shaders header: ${TR_SHADERS_HEADER}"
)
add_custom_target(TransmuteClientShadersHeader ALL
    DEPENDS ${TR_SHADERS_HEADER}
)

# Add client library
add_library(TransmuteClientLibrary STATIC
    ${TR_COMMON_SOURCE}
    ${TR_CLIENT_LIBRARY_SOURCE}
)
add_dependencies(TransmuteClientLibrary TransmuteClientShadersHeader)

# Add executable target
add_executable(TransmuteClient ${TR_CLIENT_SOURCE})
add_dependencies(TransmuteClient TransmuteClientLibrary)
target_link_libraries(TransmuteClient PRIVATE TransmuteClientLibrary)

set(NODE_ADDON_API_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/node-addon-api/include)
target_include_directories(TransmuteClient PRIVATE ${NODE_ADDON_API_HEADERS_PATH})
target_include_directories(TransmuteClientLibrary PRIVATE ${NODE_ADDON_API_HEADERS_PATH})

# Link libraries
tr_target_link_library(TransmuteClientLibrary ${TR_CRATE_BUILD_PATH} jsar_jsbindings STATIC)
tr_target_link_thirdparty_library(TransmuteClientLibrary node)
tr_target_link_skia_library(TransmuteClientLibrary)

if(APPLE)
    set(APPLE_CLIENT_DEPS
        "-framework Carbon"
        "-framework CoreFoundation"
    )
    target_link_libraries(TransmuteClientLibrary PRIVATE ${APPLE_CLIENT_DEPS})
endif()

# Set properties
tr_target_set_properties(TransmuteClient USE_EXECUTABLE_PATH True)

# Install client
tr_target_install(TransmuteClient)

# Install Node.js
tr_install_nodejs()
