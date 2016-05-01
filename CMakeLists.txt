cmake_minimum_required(VERSION 3.2)

include(CTest)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
SET(CMAKE_CXX_STANDARD 14)

add_subdirectory(src)

enable_testing()

add_subdirectory(tests)
