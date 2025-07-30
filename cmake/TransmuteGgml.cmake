# Add ggml library
set(GGML_SOURCE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/ggml")

# Configure ggml build options
set(GGML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GGML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Add ggml subdirectory
add_subdirectory(${GGML_SOURCE_DIR} ggml EXCLUDE_FROM_ALL)

# Create TransmuteGgml target
add_library(TransmuteGgml INTERFACE)
target_link_libraries(TransmuteGgml INTERFACE ggml)
target_include_directories(TransmuteGgml INTERFACE ${GGML_SOURCE_DIR}/include)

message(STATUS "GGML configured successfully")