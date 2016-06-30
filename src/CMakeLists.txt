
project(nexus)

find_package(Boost 1.58 COMPONENTS program_options system thread filesystem coroutine context REQUIRED)
find_package(ICU REQUIRED)
find_package(JavaScriptCore REQUIRED)
find_package(YaHTTP REQUIRED)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

add_subdirectory(js)
add_executable(nexus
  main.cpp nexus.cpp scheduler.cpp task.cpp object.cpp value.cpp context.cpp util.cpp
  globals/global.cpp
  globals/console.cpp
  globals/scheduler.cpp
  globals/promise.cpp
  globals/module.cpp
  globals/loader.cpp
  globals/filesystem.cpp
  globals/context.cpp
  globals/io.cpp
  globals/net.cpp
  classes/io/stream.cpp
  classes/io/filter.cpp
  classes/io/device.cpp
  classes/io/devices/file.cpp
  classes/io/devices/socket.cpp
  classes/io/filters/encoding.cpp
  classes/io/filters/utf8stringfilter.cpp
  classes/net/tcp/acceptor.cpp
  classes/net/http/server.cpp
  classes/net/http/connection.cpp
  classes/net/http2/server.cpp
  classes/context.cpp
  classes/emitter.cpp
  classes/task.cpp
  classes/base.cpp
)

target_link_libraries(nexus js_bundle ${JAVASCRIPTCORE_LIBRARY} ${Boost_LIBRARIES} ${YAHTTP_LIBRARY} ${ICU_LIBRARIES} pthread)
target_include_directories(nexus
  PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/generated/
  SYSTEM ${JAVASCRIPTCORE_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} ${ICU_INCLUDE_DIR} ${YAHTTP_INCLUDE_DIR}
)

link_directories(${Boost_LIBRARY_DIRS})

install(TARGETS nexus RUNTIME DESTINATION bin)
