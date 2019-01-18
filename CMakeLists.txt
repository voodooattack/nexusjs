cmake_minimum_required(VERSION 3.3)

set(ExternalSharedLibraries) # variable to track all dependencies

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(NEXUS_VERSION "${GIT_COMMIT_HASH}")

include(CTest)

# Remove 'lib' prefix for shared libraries on Windows
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")
#set(CMAKE_COLOR_MAKEFILE CACHE BOOL "ON")

set(PORT JSCOnly)
set(CPACK_PACKAGE_NAME Nexus)
set(CPACK_PACKAGE_VERSION "${NEXUS_VERSION}")

set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH}
)

include(cotire)

include(WebKitExternal)

include_directories(include)
add_subdirectory(src)

enable_testing()

add_subdirectory(tests)
