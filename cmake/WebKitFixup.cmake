message(STATUS "Copying WebKit")

file(COPY ${WEBKIT_ROOT} DESTINATION ${WEBKIT_BUILD_DIR}/.. REGEX ${EXCLUDE} EXCLUDE)

# list(APPEND WEBKIT_PROJECTS_NEEDED "JavaScriptCore" "bmalloc" "WTF" "WebCore")

# foreach(FILE ${WEBKIT_PROJECTS_NEEDED})
#   if (EXISTS "${WEBKIT_ROOT}/Source/${PROJ}/CMakeLists.txt")
#     file(READ "${WEBKIT_ROOT}/Source/${PROJ}/CMakeLists.txt" filedata)
#     set(filedata "project(${PROJ})\n${filedata}")
#     string(REPLACE "\${CMAKE_SOURCE_DIR}" "\${WEBKIT_BUILD_DIR}" filedata "${filedata}")
#     file(WRITE "${WEBKIT_BUILD_DIR}/Source/${PROJ}/CMakeLists.txt" "${filedata}")
#   endif() 
# endforeach()

macro(PROCESS_SCRIPT input_file)
  if (EXISTS ${input_file})
    message(STATUS "Processing ${input_file}")
    file(READ ${input_file} filedata)
    string(REPLACE "\${CMAKE_SOURCE_DIR}" "\${WEBKIT_BUILD_DIR}" filedata "${filedata}")
    string(REPLACE "add_subdirectory(PerformanceTests)" "" filedata "${filedata}")
    string(REPLACE ${WEBKIT_ROOT} "" output_file "${input_file}")
    message(STATUS "Writing ${WEBKIT_BUILD_DIR}/${output_file}")
    file(WRITE ${WEBKIT_BUILD_DIR}/${output_file} "${filedata}")
  endif()
endmacro()

file(GLOB_RECURSE ROOT_CMAKE_FILES "${WEBKIT_ROOT}/CMakeLists.txt" "${WEBKIT_ROOT}/**/CMakeLists.txt" 
  "${WEBKIT_ROOT}/**/*.cmake")

list(APPEND CMAKE_LISTS ${ROOT_CMAKE_FILES})

list(APPEND CMAKE_LISTS ${WEBKIT_ROOT}/Source/CMakeLists.txt)
list(APPEND CMAKE_LISTS ${WEBKIT_ROOT}/CMakeLists.txt)

foreach(input_file ${CMAKE_LISTS})
  # if(input_file MATCHES "ThirdParty")
    # do nothing here
  # else()
    process_script(${input_file})
  # endif()
endforeach()