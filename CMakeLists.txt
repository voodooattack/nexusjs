cmake_minimum_required(VERSION 2.6)
project(nexusjs)

add_executable(nexusjs main.cpp)

install(TARGETS nexusjs RUNTIME DESTINATION bin)
