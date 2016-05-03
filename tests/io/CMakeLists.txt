add_test(NAME encoding WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/io/encoding.js)
add_test(NAME socket_udp WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/io/socket_udp.js)
