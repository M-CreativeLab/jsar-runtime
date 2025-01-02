### Tests

file(GLOB TR_CATCH2_SOURCE
    "tests/catch2/*.cpp"
)
file(GLOB TR_COMMON_TESTS_SOURCE
    "tests/command_buffers_base/*.cpp"
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
        ${TR_COMMON_TESTS_SOURCE}
        tests/runtime.cpp
        tests/math.cpp
    )
    target_include_directories(TransmuteUnitTests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
    )

    # Add tests
    add_test(NAME CommonTests COMMAND TransmuteCommandBuffersBaseTest)
    add_test(NAME UnitTests COMMAND TransmuteUnitTests)
endif()
