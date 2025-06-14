file(GLOB TR_CORE_SOURCE
    "src/analytics/*.cpp"
    "src/math/*.cpp"
    "src/runtime/*.cpp"
    "src/renderer/*.cpp"
    "src/renderer/gles/*.cpp"
    "src/xr/*.cpp"
)

if(NOT TR_ENABLE_INSPECTOR)
    list(FILTER TR_CORE_SOURCE EXCLUDE REGEX ".*/src/runtime/inspector\\.cpp$")
    list(FILTER TR_CORE_SOURCE EXCLUDE REGEX ".*/src/runtime/inspector_[^/]*\\.cpp$")
endif()

if(APPLE)
    file(GLOB TR_CORE_SOURCE_MM "src/renderer/*.mm")
    list(APPEND TR_CORE_SOURCE ${TR_CORE_SOURCE_MM})
endif()

execute_process(
    COMMAND node ${CMAKE_SOURCE_DIR}/tools/printversion.cjs
    OUTPUT_VARIABLE TR_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
add_definitions(-DTR_VERSION="${TR_VERSION}")

set(TRANSMUTE_CORE_LIBNAME TransmuteCore)
add_library(TransmuteCore SHARED 
    ${TR_COMMON_SOURCE}
    ${TR_CORE_SOURCE}
)

# Generate the TransmuteClient binary header, which the core library will embed and install at runtime.
set(TRANSMUTE_CLIENT_BINARY_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TransmuteClient")
set(TRANSMUTE_CLIENT_BINARY_HEADER "${CMAKE_SOURCE_DIR}/src/runtime/res/client.bin.h")
set(TR_BINARY_GENERATOR "${CMAKE_SOURCE_DIR}/tools/generate_binary_header.sh")
if(ANDROID)
    set(TR_BINARY_GENERATOR_PLATFORM "android")
endif()
add_custom_command(
    OUTPUT ${TRANSMUTE_CLIENT_BINARY_HEADER}
    COMMAND ${TR_BINARY_GENERATOR} transmute_client_binary ${TRANSMUTE_CLIENT_BINARY_FILE} ${TRANSMUTE_CLIENT_BINARY_HEADER} --platform=${TR_BINARY_GENERATOR_PLATFORM}
    DEPENDS TransmuteClient
    COMMENT "Generating client binary header"
)
add_custom_target(TransmuteClientBinaryHeader DEPENDS ${TRANSMUTE_CLIENT_BINARY_HEADER})
add_dependencies(TransmuteCore TransmuteClientBinaryHeader) # Ensure the header is generated before building the core library.

# Link the libraries for the inspector
if(TR_ENABLE_INSPECTOR)
    target_link_libraries(TransmuteCore PRIVATE llhttp::llhttp)
endif()

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
if (WIN32)
    if(MSVC)
        # Arch AVX is problematic for many users, so disable it until
        # some reasonable strategy (a separate AVX target?) is determined
        #target_compile_options(${proj} PRIVATE /arch:AVX /Zi)
        target_compile_options(${TRANSMUTE_CORE_LIBNAME} PRIVATE /Zi)
    endif(MSVC)
    target_compile_definitions(${TRANSMUTE_CORE_LIBNAME} PRIVATE __WINDOWS_WASAPI__=1)
    # TODO: These vars are for libniquist and should be set in the find libynquist script.
    target_compile_definitions(${TRANSMUTE_CORE_LIBNAME} PRIVATE HAVE_STDINT_H=1 HAVE_SINF=1)
elseif (APPLE)
    set(DARWIN_LABSOUND_DEPS
        "-framework AudioToolbox"
        "-framework AudioUnit"
        "-framework Accelerate"
        "-framework CoreAudio"
        "-framework Cocoa"
    )
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE ${DARWIN_LABSOUND_DEPS})
elseif (WIN32)
    target_link_libraries(${TRANSMUTE_CORE_LIBNAME} PRIVATE dsound.lib dxguid.lib winmm.lib)
elseif (ANDROID)
    # TODO: These vars are for libnyquist and should be set in the find libynquist script.
    # TODO: libnyquist's loadabc calls getenv and setenv. That's undesirable.
    target_compile_definitions(${TRANSMUTE_CORE_LIBNAME} PRIVATE HAVE_STDINT_H=1 HAVE_SETENV=1 HAVE_SINF=1)
endif()

# Link to the LabSound library
tr_target_link_labsound_library(TransmuteCore LabSound)
tr_target_link_labsound_library(TransmuteCore samplerate)
tr_target_link_labsound_library(TransmuteCore libnyquist)

if (WIN32)
    foreach (source IN LISTS TRANSMUTE_CORE_SOURCE)
        file(RELATIVE_PATH source_rel ${CMAKE_CURRENT_LIST_DIR} ${source})
        get_filename_component(source_path "${source_rel}" PATH)
        string(REPLACE "/" "\\" source_path_msvc "${source_path}")
        source_group("${source_path_msvc}" FILES "${source}")
    endforeach ()
endif()

tr_target_set_properties(TransmuteCore)
tr_target_link_library(TransmuteCore ${TR_CRATE_BUILD_PATH} jsar_runtime_apis STATIC)
tr_target_install(TransmuteCore)

# Add examples
# TODO: move to a separate TransmuteExample.cmake?
function(tr_add_example EXECUTABLE_NAME SOURCE_FILE)
    find_package(ZLIB REQUIRED) # Search for ZLIB
    add_executable(${EXECUTABLE_NAME} ${SOURCE_FILE})
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE TRANSMUTE_STANDALONE)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE ZLIB::ZLIB)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE TransmuteCore)
    tr_target_link_thirdparty_library(${EXECUTABLE_NAME} glfw3)
    tr_target_link_skia_library(${EXECUTABLE_NAME})

    if (APPLE)
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework Cocoa")
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework CoreFoundation")
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework CoreGraphics")
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework IOKit")
        target_link_libraries(${EXECUTABLE_NAME} PRIVATE "-framework AppKit")
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
    tr_target_install(${EXECUTABLE_NAME})
endfunction()

if (APPLE)
    tr_add_example(jsar_desktop_opengl "src/examples/desktop_opengl.cpp")
endif()
