project(nexus_tests)

set(ENWIK8_URL http://mattmahoney.net/dc/enwik8.zip)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/tests/enwik8.zip")
  message(STATUS "Downloading '${ENWIK8_URL}'")
  file(DOWNLOAD ${ENWIK8_URL} ${CMAKE_BINARY_DIR}/tests/enwik8.zip)
endif()
if(NOT EXISTS "${CMAKE_BINARY_DIR}/tests/enwik8")
  message(STATUS "Extracting 'enwik.zip'")
  execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/tests/enwik8.zip
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/
  )
endif()

add_subdirectory(basic)
add_subdirectory(io)
add_subdirectory(net)
add_subdirectory(wasm)

#add_test(NAME context WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/context.js)
add_test(NAME filesystem WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/filesystem.js)
add_test(NAME module WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/module.js)
