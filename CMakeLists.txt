cmake_minimum_required(VERSION 3.2)

include(CTest)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include(submodules)
include(importSubmodules)

include_directories(include)
add_subdirectory(src)

enable_testing()

add_subdirectory(tests)

