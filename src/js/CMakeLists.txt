project(js_bundle)

# Create directory if needed
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/generated")

find_program(XXD_EXECUTABLE xxd)
file(GLOB JS_SOURCES ${CMAKE_SOURCE_DIR}/src/js/**.js)

foreach(FILE ${JS_SOURCES})
  get_filename_component(BASE_NAME ${FILE} NAME)
  file(
    GENERATE
    OUTPUT "${CMAKE_BINARY_DIR}/generated/${BASE_NAME}"
    INPUT ${FILE}
  )
  add_custom_command(
    OUTPUT  "${CMAKE_BINARY_DIR}/generated/${BASE_NAME}.inc"
    COMMAND ${XXD_EXECUTABLE} -i "${BASE_NAME}" "${CMAKE_BINARY_DIR}/generated/${BASE_NAME}.inc"
    DEPENDS ${FILE}
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/generated/"
  )
  list(APPEND JS_FILES "${CMAKE_BINARY_DIR}/generated/${BASE_NAME}.inc")
endforeach()

message(${JS_FILES})

add_library(js_bundle STATIC ${JS_FILES})
set_target_properties(js_bundle PROPERTIES LINKER_LANGUAGE CXX)
