add_test(NAME transaction WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/basic/transaction.js)
add_test(NAME atomic WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/basic/atmoic.js)
add_test(NAME promise WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/basic/promise.js)
add_test(NAME emitter WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests" COMMAND nexus ${CMAKE_SOURCE_DIR}/tests/basic/emitter.js)
