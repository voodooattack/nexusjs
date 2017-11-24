if(NOT EXISTS "${CMAKE_BINARY_DIR}/tests/enwik8")
  file(DOWNLOAD http://mattmahoney.net/dc/enwik8.zip ${CMAKE_BINARY_DIR}/tests/enwik8.zip)
  execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_BINARY_DIR}/tests/enwik8.zip
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/
  )
endif()

add_subdirectory(basic)
add_subdirectory(io)
add_subdirectory(net)

add_test(NAME context WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/context.js)
add_test(NAME filesystem WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/filesystem.js)
add_test(NAME module WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/module.js)
