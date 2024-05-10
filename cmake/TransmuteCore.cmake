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

function(transmute_add_library LIBRARY_DIRECTORY LIBRARY_NAME)
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME}
        PRIVATE ${LIBRARY_DIRECTORY}/${LIBRARY_PREFIX}${LIBRARY_NAME}.${STATICLIB_EXTNAME})
endfunction()

set(INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/transmute/_lib)
file(GLOB TRANSMUTE_CORE_SOURCE
    "src/*.cpp"
    "src/analytics/*.cpp"
    "src/bindings/env/*.cpp"
    "src/bindings/logger/*.cpp"
    "src/bindings/messaging/*.cpp"
    "src/bindings/renderer/*.cpp"
    "src/bindings/webgl/*.cpp"
    "src/bindings/webxr/*.cpp"
    "src/math/*.cpp"
    "src/runtime/*.cpp"
    "src/renderer/*.cpp"
    "src/renderer/gles/*.cpp"
    "src/xr/*.cpp"
)
if(APPLE)
    file(GLOB TRANSMUTE_CORE_SOURCE_MM "src/renderer/*.mm")
    list(APPEND TRANSMUTE_CORE_SOURCE ${TRANSMUTE_CORE_SOURCE_MM})
endif()

# file(GLOB TRANSMUTE_PROTO_SOURCE "src/proto/*.pb.cc")

set(TRANSMUTE_CORE_LIBNAME TransmuteCore)
add_library(TransmuteCore SHARED ${TRANSMUTE_CORE_SOURCE} ${TRANSMUTE_PROTO_SOURCE})

# Set the common include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/proto
    ${CMAKE_SOURCE_DIR}/thirdparty/headers
)

# Add Node.js headers
set(NODEJS_VERSION 18.12.1)
set(NODEJS_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/node-v${NODEJS_VERSION}/include)
include_directories(${NODEJS_HEADERS_PATH} ${NODEJS_HEADERS_PATH}/node)

# Add Node Addon API headers
set(NODE_ADDON_API_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/node-addon-api/include)
target_include_directories(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${NODE_ADDON_API_HEADERS_PATH})

# Add LabSound headers
set(LABSOUND_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/LabSound/include)
target_include_directories(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${LABSOUND_HEADERS_PATH})

# Add Unity headers
set(UNITY_HEADERS_PATH ${CMAKE_SOURCE_DIR}/thirdparty/headers/Unity/include)
target_include_directories(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${UNITY_HEADERS_PATH})

# Add Glm headers
include(thirdparty/headers/glm/CMakeLists.txt)
target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE glm::glm-header-only)

# Add rust crates headers & libraries
target_include_directories(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${CMAKE_SOURCE_DIR}/build/output/headers)
if (APPLE)
    set(TRANSMUTE_CRATE_TARGET universal-apple-darwin/)
elseif (ANDROID)
    set(TRANSMUTE_CRATE_TARGET aarch64-linux-android/)
elseif (WIN32)
    set(TRANSMUTE_CRATE_TARGET x86_64-pc-windows-msvc/)
endif()
message(STATUS "TRANSMUTE_CRATE_TARGET: ${TRANSMUTE_CRATE_TARGET}")
transmute_add_library(${CMAKE_SOURCE_DIR}/build/output/crates/${TRANSMUTE_CRATE_TARGET}release jsar_jsbindings)

# Optional dependencies
if (APPLE)
    set(THIRDPARTY_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/thirdparty/libs/${CMAKE_SYSTEM_NAME})
elseif (WIN32)
    set(THIRDPARTY_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/thirdparty/libs/${CMAKE_SYSTEM_NAME}/x86_64)
else()
    set(THIRDPARTY_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/thirdparty/libs/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR})
endif()
message(STATUS "Thirdparty library path: ${THIRDPARTY_LIBRARY_PATH}")

if(APPLE)
    set(APPLE_RENDERER_DEPS
        "-framework Foundation"
        "-framework CoreFoundation"
        "-framework OpenGL"
        "-framework Metal"
        # The followings are required by surfman(rust).
        "-framework QuartzCore"
        "-framework IOSurface"
        "-lobjc"
    )
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${APPLE_RENDERER_DEPS})
elseif (ANDROID)
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE GLESv3 EGL)
endif()

# LabSound libraries requirements
# See https://github.com/LabSound/LabSound/blob/main/cmake/examples.cmake
# if (WIN32)
#     if(MSVC)
#         # Arch AVX is problematic for many users, so disable it until
#         # some reasonable strategy (a separate AVX target?) is determined
#         #target_compile_options(${proj} PRIVATE /arch:AVX /Zi)
#         target_compile_options(${TRANSMUTE_CORE_LIBNAME} PRIVATE /Zi)
#     endif(MSVC)
#     target_compile_definitions(${TRANSMUTE_CORE_LIBNAME} PRIVATE __WINDOWS_WASAPI__=1)
#     # TODO: These vars are for libniquist and should be set in the find libynquist script.
#     target_compile_definitions(${TRANSMUTE_CORE_LIBNAME} PRIVATE HAVE_STDINT_H=1 HAVE_SINF=1)
# elseif (APPLE)
#     set(DARWIN_LABSOUND_DEPS
#         "-framework AudioToolbox"
#         "-framework AudioUnit"
#         "-framework Accelerate"
#         "-framework CoreAudio"
#         "-framework Cocoa"
#     )
#     target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${DARWIN_LABSOUND_DEPS})
# elseif (WIN32)
#     target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE dsound.lib dxguid.lib winmm.lib)
# elseif (ANDROID)
#     # TODO: These vars are for libnyquist and should be set in the find libynquist script.
#     # TODO: libnyquist's loadabc calls getenv and setenv. That's undesirable.
#     target_compile_definitions(${TRANSMUTE_CORE_LIBNAME} PRIVATE HAVE_STDINT_H=1 HAVE_SETENV=1 HAVE_SINF=1)
# endif()

# Link to the LabSound library
# if (WIN32)
#     target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${THIRDPARTY_LIBRARY_PATH}/lib/LabSound/LabSound.lib)
#     target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${THIRDPARTY_LIBRARY_PATH}/lib/LabSound/samplerate.lib)
#     target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${THIRDPARTY_LIBRARY_PATH}/lib/LabSound/libnyquist.lib)
# else()
#     target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${THIRDPARTY_LIBRARY_PATH}/lib/LabSound/libLabSound.a)
#     target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${THIRDPARTY_LIBRARY_PATH}/lib/LabSound/libsamplerate.a)
#     target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${THIRDPARTY_LIBRARY_PATH}/lib/LabSound/liblibnyquist.a)
# endif()

# if (APPLE)
#     target_link_options(${TRANSMUTE_CORE_LIBNAME} PRIVATE -Wl,-undefined,dynamic_lookup)
# endif()

# Link to the Node.js library
if (WIN32)
    link_directories(${THIRDPARTY_LIBRARY_PATH}/lib)
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${THIRDPARTY_LIBRARY_PATH}/lib/libnode.lib)
else()
    target_link_options(${TRANSMUTE_CORE_LIBNAME} PRIVATE -L${THIRDPARTY_LIBRARY_PATH}/lib)
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE node)
endif()

if (APPLE)
    set_target_properties(${TRANSMUTE_CORE_LIBNAME} PROPERTIES
        INSTALL_RPATH "@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
        SUFFIX ".dylib"
    )
elseif (ANDROID)
    target_link_options(${TRANSMUTE_CORE_LIBNAME} PRIVATE -Wl,-rpath,'$ORIGIN')
    set_target_properties(${TRANSMUTE_CORE_LIBNAME} PROPERTIES
        SUFFIX ".so"
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
elseif (WIN32)
    set_target_properties(${TRANSMUTE_CORE_LIBNAME} PROPERTIES
        SUFFIX ".dll"
    )
endif ()

if (ANDROID)
    # Android has a custom toolchain so pthread is not available and should
    # link against other libraries as well for logcat and internal features.
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE android log)
elseif (LINUX)
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE X11 pthread)
elseif (WIN32)
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE Winmm)
endif()

foreach (source IN LISTS TRANSMUTE_CORE_SOURCE)
    file(RELATIVE_PATH source_rel ${CMAKE_CURRENT_LIST_DIR} ${source})
    get_filename_component(source_path "${source_rel}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach ()

# Install the library
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build/output/release)
set(TRANSMUTE_RELEASE_DEST unknown)

if (APPLE)
    set(TRANSMUTE_RELEASE_DEST universal-apple-darwin)
elseif (ANDROID)
    set(TRANSMUTE_RELEASE_DEST aarch64-linux-android)
elseif (WIN32)
    set(TRANSMUTE_RELEASE_DEST x86_64-pc-windows-msvc)
endif()

# Install project's library
install(TARGETS ${TRANSMUTE_CORE_LIBNAME} DESTINATION ${TRANSMUTE_RELEASE_DEST})

# Install the dependencies
if (APPLE)
    install(FILES ${THIRDPARTY_LIBRARY_PATH}/lib/libnode.dylib DESTINATION ${TRANSMUTE_RELEASE_DEST})
    install(FILES ${THIRDPARTY_LIBRARY_PATH}/lib/libnode.108.dylib DESTINATION ${TRANSMUTE_RELEASE_DEST})
elseif (ANDROID)
    install(FILES ${THIRDPARTY_LIBRARY_PATH}/lib/libnode.so DESTINATION ${TRANSMUTE_RELEASE_DEST})
elseif (WIN32)
    install(FILES ${THIRDPARTY_LIBRARY_PATH}/lib/libnode.dll DESTINATION ${TRANSMUTE_RELEASE_DEST})
endif()

# Add Tools Function
function(ADD_JSAR_TOOL EXECUTABLE_NAME SOURCE_FILE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE TRANSMUTE_STANDALONE)
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${NODE_ADDON_API_HEADERS_PATH})
    target_include_directories(${EXECUTABLE_NAME} PRIVATE /opt/homebrew/Cellar/glfw/3.4/include)

    target_link_options(${EXECUTABLE_NAME} PRIVATE -L/opt/homebrew/Cellar/glfw/3.4/lib)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${TRANSMUTE_CORE_LIBNAME} glfw)

    if (APPLE)
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework OpenGL")
        set_target_properties(${EXECUTABLE_NAME} PROPERTIES
            INSTALL_RPATH "@loader_path"
            BUILD_WITH_INSTALL_RPATH ON
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    elseif (WIN32)
        set_target_properties(${EXECUTABLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()

    if (WIN32)
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${THIRDPARTY_LIBRARY_PATH}/lib/libnode.lib)
    endif()
endfunction()

# Add Tools
if (APPLE)
    # ADD_JSAR_TOOL(test-jsar "src/tools/tester.cpp")
    # ADD_JSAR_TOOL(bench-jsar "src/tools/bench.cpp")
    # Install the libraries to the build directory.
    install(FILES ${THIRDPARTY_LIBRARY_PATH}/lib/libnode.108.dylib DESTINATION ${CMAKE_BINARY_DIR})
elseif (WIN32)
    # ADD_JSAR_TOOL(test-jsar "Source/tools/tester.cpp")
    # Install the libraries to the build directory.
    install(FILES ${THIRDPARTY_LIBRARY_PATH}/lib/libnode.lib DESTINATION ${CMAKE_BINARY_DIR}/Release)
    install(FILES ${THIRDPARTY_LIBRARY_PATH}/lib/libnode.dll DESTINATION ${CMAKE_BINARY_DIR}/Release)
endif()
