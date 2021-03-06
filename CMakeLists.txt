# General project setup
cmake_minimum_required (VERSION 3.1)
project (PolkadotCppAPI C CXX)
message("Building project ${PROJECT_NAME}")
include(CTest)

# Set compiler flags
set(CMAKE_C_FLAGS "-ggdb -ldl")
set(CMAKE_BUILD_TYPE Debug)
if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
	endif ()
else ()
    set (CMAKE_CXX_STANDARD 11)
endif ()


if(WIN32)
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /Gy /GF /Ox /Ob2 /Ot /Oi /MP /arch:SSE2 /fp:fast")
	set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG /INCREMENTAL:NO /OPT:REF /OPT:ICF")
	add_definitions (/W3 /wd4996 /wd4995 /wd4355)
	add_definitions (-DUNICODE -D_UNICODE)
	add_definitions (-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
	add_definitions (-DNOMINMAX)
endif()


# Take care of libpthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

###############################################################################
# Dependencies:
#   - WebSocket++
#   - Boost
#   - OpenSSL
#

set (WEBSOCKETPP_BOOST_LIBS system thread)

if (WIN32)
	set (WEBSOCKETPP_INCLUDE_DIR "c:/websocketpp")
	find_package(cUrl)
	add_definitions( -DBOOST_ALL_NO_LIB )
	add_definitions( -DBOOST_ALL_DYN_LINK )

else ()
	set (WEBSOCKETPP_INCLUDE_DIR "/usr/local/include")

endif()

set (Boost_FIND_REQUIRED TRUE)
set (Boost_FIND_QUIETLY TRUE)
set (Boost_DEBUG FALSE)
set (Boost_USE_MULTITHREADED TRUE)

set (Boost_ADDITIONAL_VERSIONS "1.39.0" "1.40.0" "1.41.0" "1.42.0" "1.43.0" "1.44.0" "1.46.1" "1.65") # todo: someone who knows better specify these!

find_package(OpenSSL)

if (WIN32)
	find_package (Boost 1.65.0 COMPONENTS "${WEBSOCKETPP_BOOST_LIBS}" random regex)
	include_directories (${CURL_INCLUDE_DIRS})
else ()
	find_package (Boost 1.39.0 COMPONENTS "${WEBSOCKETPP_BOOST_LIBS}")
endif()


if (NOT Boost_FOUND)
    message (FATAL_ERROR "Failed to find required dependency: boost")
endif ()
if (NOT OpenSSL_FOUND)
    message (FATAL_ERROR "Failed to find required dependency: OpenSSL")
endif ()

if (WIN32)
	set (DEPENDENCY_LIBS ${Boost_LIBRARIES} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} Threads::Threads sr25519crust Boost::dynamic_linking Userenv)
else ()
	set (DEPENDENCY_LIBS ${Boost_LIBRARIES} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} Threads::Threads sr25519crust dl)
endif()


include_directories (${Boost_INCLUDE_DIR})
include_directories (${OPENSSL_INCLUDE_DIR})
include_directories (${WEBSOCKETPP_INCLUDE_DIR})


# End of Dependencies
#########################################################

# Setup source files directories
file(GLOB_RECURSE examples ${PROJECT_SOURCE_DIR}/examples/*.cpp)
file(GLOB_RECURSE src ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE srctest ${PROJECT_SOURCE_DIR}/test/*.cpp)

# Setup output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

###############################################################################
# Polkadot C++ API Library

# Build library code from src folder as shared object
add_library(polkacpp ${src})

###############################################################################
# Unit tests

# Build all files in test folder as a separate executable
file( GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cpp )
foreach( testsourcefile ${TEST_SOURCES} )
    string( REPLACE ".cpp" "" testfullpath ${testsourcefile} )
    string( REPLACE  "${PROJECT_SOURCE_DIR}/test/" "" testname ${testfullpath} )

    add_executable( ${testname} ${testsourcefile} )
    target_link_libraries( ${testname} polkacpp )

    add_test(${testname} "bin/${testname}")
    set_tests_properties (${testname} PROPERTIES PASS_REGULAR_EXPRESSION "success*")

    # Link dependency libraries to test executables
    target_link_libraries( ${testname} ${DEPENDENCY_LIBS} )
endforeach( testsourcefile ${TEST_SOURCES} )

###############################################################################
# Linter

# get all project files
file( GLOB_RECURSE ALL_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*)

# exclude libs folder that contains 3rd party code

set(PROJECT_TRDPARTY_DIR ${PROJECT_SOURCE_DIR}/src/libs)

foreach (SOURCE_FILE ${ALL_SOURCE_FILES})
    string(FIND ${SOURCE_FILE} ${PROJECT_TRDPARTY_DIR} PROJECT_TRDPARTY_DIR_FOUND)
    if (NOT ${PROJECT_TRDPARTY_DIR_FOUND} EQUAL -1)
        list(REMOVE_ITEM ALL_SOURCE_FILES ${SOURCE_FILE})
    endif ()
endforeach ()

# Add clang-format target
add_custom_target(
        format
        COMMAND /usr/bin/clang-format
        -style=file
        -i
        ${ALL_SOURCE_FILES}
)

###############################################################################
# Library installation

install(FILES bin/libpolkacpp.a DESTINATION /usr/lib/polkacpp)

file( GLOB_RECURSE INCLUDE_FILES ${PROJECT_SOURCE_DIR}/src/interfaces/*)
install(FILES ${INCLUDE_FILES} DESTINATION /usr/include/polkacpp/interfaces)

file( GLOB_RECURSE STRUCT_FILES ${PROJECT_SOURCE_DIR}/src/structs/*)
install(FILES ${STRUCT_FILES} DESTINATION /usr/include/polkacpp/structs)

install(FILES ${PROJECT_SOURCE_DIR}/src/polkacpp.h DESTINATION /usr/include/polkacpp)
install(FILES ${PROJECT_SOURCE_DIR}/src/scale.h DESTINATION /usr/include/polkacpp)

file( GLOB_RECURSE INT128_FILES ${PROJECT_SOURCE_DIR}/src/libs/int128/*.h)
install(FILES ${INT128_FILES} DESTINATION /usr/include/polkacpp/libs/int128)

file( GLOB_RECURSE UTIL_FILES ${PROJECT_SOURCE_DIR}/src/utils/concurrentmapqueue.h)
install(FILES ${UTIL_FILES} DESTINATION /usr/include/polkacpp/utils)

###############################################################################
# Stand-alone Example

# Build example sources as executables
add_executable(clip EXCLUDE_FROM_ALL ${examples})
target_link_libraries (clip polkacpp)

# Link dependency libraries to example project
target_link_libraries( clip ${DEPENDENCY_LIBS} )

###############################################################################
# Doxygen

add_custom_target(
        doc
        COMMAND rm -rf doc/html && doxygen
        .doxygen.conf
)

###############################################################################
# Install dependencies

add_custom_target(
        deps
        COMMAND cd .. &&
                rm -rf wpp &&
                git clone https://github.com/zaphoyd/websocketpp wpp &&
                sudo rm -rf /usr/local/include/websocketpp &&
                sudo mv wpp/websocketpp /usr/local/include/websocketpp

        # sr25519-crust
        COMMAND cd .. &&
                #sudo rm -rf sr25519 &&
                git clone https://github.com/Warchant/sr25519-crust.git sr25519 || true &&
                cd sr25519 &&
                git checkout c8df3201a29efa2c4e35751a9866f1a770bb7e1b &&
                curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain nightly &&
                rustup install nightly &&
                rustup default nightly &&
                mkdir build || true &&
                cd build &&
                cmake .. -DCMAKE_BUILD_TYPE=Release &&
                sudo -E env "PATH=/usr/bin:$ENV{HOME}/.cargo/bin:$ENV{PATH}" make install
)

###############################################################################
# Make package

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "DEB1")
INCLUDE(CPack)

function (add_package TARGET_NAME TARGET_PATH DESCR)
    install(TARGETS "${TARGET_NAME}"
            DESTINATION "${TARGET_PATH}"
            COMPONENT "${TARGET_NAME}")

    cpack_add_component_group("${TARGET_NAME}")

    cpack_add_component("${TARGET_NAME}"
                        DISPLAY_NAME "${TARGET_NAME}"
                        DESCRIPTION "${DESCR}"
                        GROUP "${TARGET_NAME}"
                        INSTALL_TYPES Full)
endfunction ()

target_link_libraries( ${testname} ${DEPENDENCY_LIBS} )
add_package(${testname} "polkadot" "Polkadot API")
