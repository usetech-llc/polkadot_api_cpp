# General project setup
cmake_minimum_required (VERSION 3.1)
project (PolkadotCppAPI)
message("Building project ${PROJECT_NAME}")
include(CTest)

# Set compiler flags
set(CMAKE_C_FLAGS "-ggdb -ldl")
set(CMAKE_BUILD_TYPE Debug)

# Take care of libpthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



#########################################################
# WebSocket++ library

############ Paths

set (WEBSOCKETPP_BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set (WEBSOCKETPP_BIN ${WEBSOCKETPP_BUILD_ROOT}/bin)
set (WEBSOCKETPP_LIB ${WEBSOCKETPP_BUILD_ROOT}/lib)

set (WEBSOCKETPP_INCLUDE_DIR "/usr/local/include")

############

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set (DEF_INSTALL_CMAKE_DIR lib/cmake/websocketpp)
set (INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach (p INCLUDE CMAKE)
  set (var INSTALL_${p}_DIR)
  if (NOT IS_ABSOLUTE "${${var}}")
    set (${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif ()
endforeach ()

# Set CMake library search policy
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    cmake_policy (SET CMP0005 NEW)
endif ()

# Disable unnecessary build types
set (CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo;Debug" CACHE STRING "Configurations" FORCE)

# Include our cmake macros
#set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#include (CMakeHelpers)


############  Build customization

# Override from command line "CMake -D<OPTION>=TRUE/FALSE/0/1/ON/OFF"
option (ENABLE_CPP11 "Build websocketpp with CPP11 features enabled." TRUE)
option (BUILD_EXAMPLES "Build websocketpp examples." TRUE)
option (BUILD_TESTS "Build websocketpp tests." FALSE)

if (BUILD_TESTS OR BUILD_EXAMPLES)

    enable_testing ()

    ############ Compiler specific setup

    set (WEBSOCKETPP_PLATFORM_LIBS "")
    set (WEBSOCKETPP_PLATFORM_TLS_LIBS "")
    set (WEBSOCKETPP_BOOST_LIBS "")

    # VC9 and C++11 reasoning
    if (ENABLE_CPP11 AND MSVC AND MSVC90)
        message("* Detected Visual Studio 9 2008, disabling C++11 support.")
        set (ENABLE_CPP11 FALSE)
    endif ()

    # Detect clang. Not officially reported by cmake.
    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "-v" ERROR_VARIABLE CXX_VER_STDERR)
    if ("${CXX_VER_STDERR}" MATCHES ".*clang.*")
        set (CMAKE_COMPILER_IS_CLANGXX 1)
    endif ()

    # C++11 defines
    if (ENABLE_CPP11)
        if (MSVC)
            add_definitions (-D_WEBSOCKETPP_CPP11_FUNCTIONAL_)
            add_definitions (-D_WEBSOCKETPP_CPP11_SYSTEM_ERROR_)
            add_definitions (-D_WEBSOCKETPP_CPP11_RANDOM_DEVICE_)
            add_definitions (-D_WEBSOCKETPP_CPP11_MEMORY_)
        else()
            add_definitions (-D_WEBSOCKETPP_CPP11_STL_)
        endif()
    endif ()

    # Visual studio
    if (MSVC)
        set (WEBSOCKETPP_BOOST_LIBS system thread)
        set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL /Gy /GF /Ox /Ob2 /Ot /Oi /MP /arch:SSE2 /fp:fast")
        set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG /INCREMENTAL:NO /OPT:REF /OPT:ICF")
        add_definitions (/W3 /wd4996 /wd4995 /wd4355)
        add_definitions (-DUNICODE -D_UNICODE)
        add_definitions (-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
        add_definitions (-DNOMINMAX)
    endif ()

    # g++
    if (CMAKE_COMPILER_IS_GNUCXX)
        set (WEBSOCKETPP_PLATFORM_LIBS pthread rt)
        set (WEBSOCKETPP_PLATFORM_TLS_LIBS ssl crypto)
        set (WEBSOCKETPP_BOOST_LIBS system thread)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        if (NOT APPLE)
            add_definitions (-DNDEBUG -Wall -Wcast-align) # todo: should we use CMAKE_C_FLAGS for these?
        endif ()

        # Try to detect version. Note: Not tested!
        execute_process (COMMAND ${CMAKE_CXX_COMPILER} "-dumpversion" OUTPUT_VARIABLE GCC_VERSION)
        if ("${GCC_VERSION}" STRGREATER "4.4.0")
            message("* C++11 support partially enabled due to GCC version ${GCC_VERSION}")
            set (WEBSOCKETPP_BOOST_LIBS system thread)
        endif ()
    endif ()

    # clang
    if (CMAKE_COMPILER_IS_CLANGXX)
        if (NOT APPLE)
            set (WEBSOCKETPP_PLATFORM_LIBS pthread rt)
        else()
            set (WEBSOCKETPP_PLATFORM_LIBS pthread)
        endif()
        set (WEBSOCKETPP_PLATFORM_TLS_LIBS ssl crypto)
        set (WEBSOCKETPP_BOOST_LIBS system thread)
        set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++0x -stdlib=libc++") # todo: is libc++ really needed here?
        if (NOT APPLE)
            add_definitions (-DNDEBUG -Wall -Wno-padded) # todo: should we use CMAKE_C_FLAGS for these?
        endif ()
    endif ()

    # OSX, can override above.
    if (APPLE)
        add_definitions (-DNDEBUG -Wall)
    endif ()

    if (BUILD_EXAMPLES)
        list (APPEND WEBSOCKETPP_BOOST_LIBS random)
    endif()

    if (BUILD_TESTS)
        list (APPEND WEBSOCKETPP_BOOST_LIBS unit_test_framework)
    endif()

    ############ Dependencies

    # Set BOOST_ROOT env variable or pass with cmake -DBOOST_ROOT=path.
    # BOOST_ROOT can also be defined by a previous run from cmake cache.
    if (NOT "$ENV{BOOST_ROOT_CPP11}" STREQUAL "")
        # Scons documentation for BOOST_ROOT_CPP11:
        # "look for optional second boostroot compiled with clang's libc++ STL library
        # this prevents warnings/errors when linking code built with two different
        # incompatible STL libraries."
        file (TO_CMAKE_PATH "$ENV{BOOST_ROOT_CPP11}" BOOST_ROOT)
        set (BOOST_ROOT ${BOOST_ROOT} CACHE PATH "BOOST_ROOT dependency path" FORCE)
    endif ()
    if ("${BOOST_ROOT}" STREQUAL "")
        file (TO_CMAKE_PATH "$ENV{BOOST_ROOT}" BOOST_ROOT)
        # Cache BOOST_ROOT for runs that do not define $ENV{BOOST_ROOT}.
        set (BOOST_ROOT ${BOOST_ROOT} CACHE PATH "BOOST_ROOT dependency path" FORCE)
    endif ()

    message ("* Configuring Boost")
    message (STATUS "-- Using BOOST_ROOT")
    message (STATUS "       " ${BOOST_ROOT})

    if (MSVC)
        set (Boost_USE_MULTITHREADED TRUE)
        set (Boost_USE_STATIC_LIBS TRUE)
    else ()
        set (Boost_USE_MULTITHREADED FALSE)
        set (Boost_USE_STATIC_LIBS FALSE)
    endif ()

	if (BOOST_STATIC)
		set (Boost_USE_STATIC_LIBS TRUE)
	endif ()

    if (NOT Boost_USE_STATIC_LIBS)
        add_definitions (/DBOOST_TEST_DYN_LINK)
    endif ()

    set (Boost_FIND_REQUIRED TRUE)
    set (Boost_FIND_QUIETLY TRUE)
    set (Boost_DEBUG FALSE)
    set (Boost_USE_MULTITHREADED TRUE)
    set (Boost_ADDITIONAL_VERSIONS "1.39.0" "1.40.0" "1.41.0" "1.42.0" "1.43.0" "1.44.0" "1.46.1") # todo: someone who knows better spesify these!

    find_package (Boost 1.39.0 COMPONENTS "${WEBSOCKETPP_BOOST_LIBS}")

    if (Boost_FOUND)
        # Boost is a project wide global dependency.
        include_directories (${Boost_INCLUDE_DIRS})
        link_directories (${Boost_LIBRARY_DIRS})

        # Pretty print status
        message (STATUS "-- Include Directories")
        foreach (include_dir ${Boost_INCLUDE_DIRS})
            message (STATUS "       " ${include_dir})
        endforeach ()
        message (STATUS "-- Library Directories")
        foreach (library_dir ${Boost_LIBRARY_DIRS})
            message (STATUS "       " ${library_dir})
        endforeach ()
        message (STATUS "-- Libraries")
        foreach (boost_lib ${Boost_LIBRARIES})
            message (STATUS "       " ${boost_lib})
        endforeach ()
        message ("")
    else ()
        message (FATAL_ERROR "Failed to find required dependency: boost")
    endif ()

    find_package(OpenSSL)
    find_package(ZLIB)
endif()

export (PACKAGE websocketpp)

#########################################################

# Setup source files directories
file(GLOB_RECURSE examples ${PROJECT_SOURCE_DIR}/examples/*.cpp)
file(GLOB_RECURSE src ${PROJECT_SOURCE_DIR}/src/*.cpp)
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

##### Link Boost and OpenSSL to example project

target_link_libraries( polkaclient ${Boost_LIBRARIES} )
target_link_libraries( polkaclient ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} )
target_link_libraries( polkaclient Threads::Threads )
target_link_libraries( polkaclient sr25519crust )
target_link_libraries( polkaclient dl )

set_property(TARGET polkaclient APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
set_property(TARGET polkaclient APPEND PROPERTY INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
set_property(TARGET polkaclient APPEND PROPERTY INCLUDE_DIRECTORIES ${WEBSOCKETPP_INCLUDE_DIR})

#####



# Unit Testing - Build all files in test folder as a separate test
file( GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/test/*.cpp )
foreach( testsourcefile ${TEST_SOURCES} )
    string( REPLACE ".cpp" "" testfullpath ${testsourcefile} )
    string( REPLACE  "${PROJECT_SOURCE_DIR}/test/" "" testname ${testfullpath} )

    add_executable( ${testname} ${testsourcefile} )
    target_link_libraries( ${testname} polkacpp )

    add_test(${testname} "bin/${testname}")
    set_tests_properties (${testname} PROPERTIES PASS_REGULAR_EXPRESSION "success*")

    # link Boost and OpenSSL to test executables
    target_link_libraries( ${testname} ${Boost_LIBRARIES} )
    target_link_libraries( ${testname} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} )
    target_link_libraries( ${testname} Threads::Threads )
    target_link_libraries( ${testname} sr25519crust )
    target_link_libraries( ${testname} dl )
    set_property(TARGET ${testname} APPEND PROPERTY INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
    set_property(TARGET ${testname} APPEND PROPERTY INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
    set_property(TARGET ${testname} APPEND PROPERTY INCLUDE_DIRECTORIES ${WEBSOCKETPP_INCLUDE_DIR})

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
                curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain nightly &&
                rustup install nightly &&
                rustup default nightly &&
                mkdir build || true &&
                cd build &&
                cmake .. -DCMAKE_BUILD_TYPE=Release &&
                sudo -E env "PATH=/usr/bin:$ENV{HOME}/.cargo/bin:$ENV{PATH}" make install
)
