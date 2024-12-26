
### Tests

file(GLOB TR_CATCH2_SOURCE
    "tests/catch2/*.cpp"
)

# check for TR_BUILD_TESTS
if (TR_BUILD_TESTS)
    message(STATUS "Building tests")
    enable_testing()

    add_executable(TransmuteCommonTest
        tests/common.cpp
        ${TR_COMMON_SOURCE}
    )
    add_executable(TransmuteUnitTests
        ${TR_CATCH2_SOURCE}
        tests/runtime.cpp
    )
    target_include_directories(TransmuteUnitTests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
    )

    # Add tests
    add_test(NAME CommonTests COMMAND TransmuteCommonTest)
    add_test(NAME UnitTests COMMAND TransmuteUnitTests)
endif()
