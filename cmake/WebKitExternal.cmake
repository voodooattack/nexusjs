find_program(SVN_EXECUTABLE svn)

set(WEBKIT_REPO_URL "http://svn.webkit.org/repository/webkit/trunk")
set(WEBKIT_REPO_REV "239908")
set(WEBKIT_ROOT "${CMAKE_SOURCE_DIR}/vendor/webkit")
set(WEBKIT_PATCH_DIR "${CMAKE_SOURCE_DIR}/patch/webkit")
set(WEBKIT_BUILD_DIR "${CMAKE_BINARY_DIR}/vendor/webkit")

if(SVN_EXECUTABLE)
  message(STATUS "Updating WebKit")
  execute_process(
      COMMAND ${SVN_EXECUTABLE} cleanup
      WORKING_DIRECTORY ${WEBKIT_ROOT}
      OUTPUT_QUIET
      ERROR_QUIET
  )

  execute_process(
      COMMAND ${SVN_EXECUTABLE} checkout "${WEBKIT_REPO_URL}" --revision ${WEBKIT_REPO_REV} ${WEBKIT_ROOT}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vendor
  )

  execute_process(
      COMMAND ${SVN_EXECUTABLE} revert --recursive .
      WORKING_DIRECTORY ${WEBKIT_ROOT}
      OUTPUT_QUIET
      ERROR_QUIET
  )

  file(GLOB_RECURSE PATCHES "${WEBKIT_PATCH_DIR}/*.patch")

  message(STATUS "Patching WebKit")
  foreach (PATCH ${PATCHES})
    message(STATUS "Applying patch ${PATCH} to WebKit")
    execute_process(
        COMMAND ${SVN_EXECUTABLE} patch ${PATCH}
        WORKING_DIRECTORY ${WEBKIT_ROOT}
        OUTPUT_QUIET
    )
  endforeach ()
endif()

# big hack because patching this would be useless!

execute_process(
    COMMAND ${CMAKE_COMMAND} "-DWEBKIT_ROOT=${WEBKIT_ROOT}" "-DWEBKIT_BUILD_DIR=${WEBKIT_BUILD_DIR}"
      "-DEXCLUDE=(Layout|JS)Tests|\\.svn" -P "${CMAKE_SOURCE_DIR}/cmake/WebKitFixup.cmake"
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${WEBKIT_BUILD_DIR}/Source/cmake)

set(JAVASCRIPTCORE_DIR ${WEBKIT_BUILD_DIR}/Source/JavaScriptCore)
set(JavaScriptCore_LIBRARY_TYPE "STATIC")
set(WebKit_LIBRARY_TYPE "STATIC")
set(WebCore_LIBRARY_TYPE "STATIC")
set(WTF_LIBRARY_TYPE "STATIC")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PORT "JSCOnly" CACHE STRING "" FORCE)
set(ENABLE_STATIC_JSC ON CACHE BOOL "" FORCE)
set(EVENT_LOOP_TYPE "Generic" CACHE STRING "" FORCE)
set(USE_SYSTEM_MALLOC OFF CACHE BOOL "" FORCE)
set(ENABLE_API_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

set(CMAKE_MODULE_PATH
    ${WEBKIT_ROOT}/Source/cmake
    ${CMAKE_MODULE_PATH}
    )
set(THIRDPARTY_DIR "${WEBKIT_BUILD_DIR}/Source/ThirdParty")

add_subdirectory(${WEBKIT_BUILD_DIR} ${CMAKE_BINARY_DIR}/build/webkit EXCLUDE_FROM_ALL)

set(CMAKE_MODULE_PATH
    ${WEBKIT_BUILD_DIR}/Source/cmake
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH}
    )

set(JSC_PUBLIC_HEADERS_SRC ${WEBKIT_BUILD_DIR}/Source/JavaScriptCore/API)
set(JSC_PUBLIC_HEADERS ${CMAKE_BINARY_DIR}/vendor/include)

make_directory(${JSC_PUBLIC_HEADERS}/JavaScriptCore)

SET(JAVASCRIPTCORE_INCLUDE_DIR ${WEBKIT_BUILD_DIR}/Source)

SET(JAVASCRIPTCORE_INCLUDE_DIR
    ${JSC_PUBLIC_HEADERS}
    ${JAVASCRIPTCORE_INCLUDE_DIR}
    ${JAVASCRIPTCORE_INCLUDE_DIR}/bmalloc
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/yarr
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/llint
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/heap
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/parser
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/domjit
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/dfg
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/profiler
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/interpreter
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/runtime
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/jit
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/assembler
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/wasm
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/wasm/js
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/bytecode
    ${JAVASCRIPTCORE_INCLUDE_DIR}/JavaScriptCore/API
    ${JAVASCRIPTCORE_INCLUDE_DIR}/WTF
    ${WTF_INCLUDE_DIR}
    ${WEBKIT_BUILD_DIR}
    ${CMAKE_BINARY_DIR}/DerivedSources/JavaScriptCore
    )
