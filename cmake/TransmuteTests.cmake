### Tests

file(GLOB TR_CATCH2_SOURCE
    "tests/catch2/*.cpp"
)
file(GLOB TR_COMMON_TESTS_SOURCE
    "tests/**/*_tests.cpp"
)
file(GLOB_RECURSE TR_CLIENT_BUILTIN_SCENE_SOURCE
    "src/client/builtin_scene/*.cpp"
)

# check for TR_BUILD_TESTS
if (TR_BUILD_TESTS)
    message(STATUS "Building tests")
    enable_testing()

    add_executable(TransmuteCommandBuffersBaseTest
        tests/command_buffers_base.cpp
        ${TR_COMMON_SOURCE}
    )
    add_executable(TransmuteUnitTests
        ${TR_CATCH2_SOURCE}
        ${TR_COMMON_SOURCE}
        ${TR_COMMON_TESTS_SOURCE}
        ${TR_CLIENT_BUILTIN_SCENE_SOURCE}
        tests/runtime.cpp
        tests/math.cpp
    )
    target_include_directories(TransmuteUnitTests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
        ${CMAKE_SOURCE_DIR}/thirdparty/headers/node-addon-api/include
    )

    # Add tests
    add_test(NAME CommonTests COMMAND TransmuteCommandBuffersBaseTest)
    add_test(NAME UnitTests COMMAND TransmuteUnitTests)
endif()
