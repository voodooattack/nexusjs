enable_testing()

add_test(NAME transaction WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/transaction.js)
add_test(NAME atomic WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/atmoic.js)
add_test(NAME promise WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/promise.js)
add_test(NAME io WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/io.js)
