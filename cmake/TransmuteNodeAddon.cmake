if (ANDROID)
    message("Skip building the addon if the platform is android, this is only works for development platform")
    return ()
endif ()

file(GLOB TRANSMUTE_NAPI_SOURCE
    "Source/transmute/vector.cpp"
    "Source/transmute/mesh.cpp"
    "Source/transmute/gom/*.cpp"
    "Source/transmute/gom.cpp"
    "Source/transmute/gom_binding.cpp"
)
file(GLOB TRANSMUTE_PROTO_SOURCE "Source/proto/*.pb.cc")

add_library(TransmuteNodeAddon SHARED ${TRANSMUTE_NAPI_SOURCE} ${TRANSMUTE_PROTO_SOURCE})

target_include_directories(TransmuteNodeAddon PRIVATE ${CMAKE_SOURCE_DIR}/Source)
target_include_directories(TransmuteNodeAddon PRIVATE ${CMAKE_SOURCE_DIR}/Source/transmute)

# Add Node.js headers
set(NODEJS_VERSION 18.12.1)
set(NODEJS_HEADERS_PATH ${CMAKE_SOURCE_DIR}/Thirdparty/Headers/node-v${NODEJS_VERSION}/include)
target_include_directories(TransmuteNodeAddon PRIVATE ${NODEJS_HEADERS_PATH})
target_include_directories(TransmuteNodeAddon PRIVATE ${NODEJS_HEADERS_PATH}/node)

# Add Node Addon API headers
set(NODE_ADDON_API_HEADERS_PATH ${CMAKE_SOURCE_DIR}/Thirdparty/Headers/node-addon-api/include)
target_include_directories(TransmuteNodeAddon PRIVATE ${NODE_ADDON_API_HEADERS_PATH})

# Add Protobuf C++ headers
set(PROTOBUF_VERSION 21.12)
set(PROTOBUF_HEADERS_PATH ${CMAKE_SOURCE_DIR}/Thirdparty/Headers/protobuf-cpp-v${PROTOBUF_VERSION}/include)
target_include_directories(TransmuteNodeAddon PRIVATE ${PROTOBUF_HEADERS_PATH})

# Optional dependencies
if (APPLE)
    set(THIRDPARTY_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/Thirdparty/Library/${CMAKE_SYSTEM_NAME})
else()
    set(THIRDPARTY_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/Thirdparty/Library/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR})
endif()

target_link_options(TransmuteNodeAddon PRIVATE -L${THIRDPARTY_LIBRARY_PATH}/lib)
if (APPLE)
    target_link_options(TransmuteNodeAddon PRIVATE -Wl,-undefined,dynamic_lookup)
elseif (UNIX)
    target_link_options(TransmuteNodeAddon PRIVATE -Wl,--unresolved-symbols=ignore-all)
elseif (WIN32)
    target_link_options(TransmuteNodeAddon PRIVATE /FORCE)
endif ()

target_link_libraries(TransmuteNodeAddon PRIVATE protobuf-lite)

if (APPLE)
    set_target_properties(TransmuteNodeAddon PROPERTIES
        INSTALL_RPATH "@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
        OUTPUT_NAME "transmute"
        PREFIX ""
        SUFFIX ".node"
    )
elseif (ANDROID)
    target_link_options(TransmuteNodeAddon PRIVATE -Wl,-rpath,'$ORIGIN')
    set_target_properties(TransmuteNodeAddon PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
        OUTPUT_NAME "transmute"
        PREFIX ""
        SUFFIX ".node"
    )
endif ()

if (ANDROID)
    # Android has a custom toolchain so pthread is not available and should
    # link against other libraries as well for logcat and internal features.
    target_link_libraries(TransmuteNodeAddon PRIVATE android log)
elseif (LINUX)
    target_link_libraries(TransmuteNodeAddon PRIVATE X11 pthread)
elseif (WIN32)
    target_link_libraries(TransmuteNodeAddon PRIVATE Winmm)
endif()

# Install the library
if (APPLE)
    set(ADDON_INSTALL_PATH ${CMAKE_SOURCE_DIR}/Source/jsframework/devtools/vscode-extension/binding)
    install(TARGETS TransmuteNodeAddon DESTINATION ${ADDON_INSTALL_PATH})
    install(FILES ${THIRDPARTY_LIBRARY_PATH}/lib/libprotobuf-lite.32.dylib DESTINATION ${ADDON_INSTALL_PATH})
endif()
