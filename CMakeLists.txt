# General project setup
cmake_minimum_required (VERSION 3.0)
project (PolkadotCppAPI)
message("Building project ${PROJECT_NAME}")
include(CTest)

# Set compiler flags
set(CMAKE_C_FLAGS "-ggdb")
set(CMAKE_BUILD_TYPE Debug)

# Setup source files directories
file(GLOB_RECURSE examples ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE src ${PROJECT_SOURCE_DIR}/examples/*.cpp)
file(GLOB_RECURSE srctest ${PROJECT_SOURCE_DIR}/test/*.cpp)

# Setup output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build library code from src folder as shared object
add_library(polkacpp ${src})

# Build example sources as executables
add_executable(polkaclient ${examples})
target_link_libraries (polkaclient polkacpp)

# Unit Testing
add_executable(tests ${srctest})
add_test(TEST01 bin/tests)
set_tests_properties (TEST01 PROPERTIES PASS_REGULAR_EXPRESSION "success*")
