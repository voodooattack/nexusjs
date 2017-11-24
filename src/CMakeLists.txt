
project(nexus)

find_package(Boost 1.62 COMPONENTS program_options system thread filesystem coroutine context REQUIRED)
find_package(ICU REQUIRED)

if (MSVC)
  # Force to always compile with W4
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif ()
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long -Wno-expansion-to-defined")
endif ()

add_subdirectory(js)

set(INCLUDES
    ../include/nexus.h
    ../include/context.h
    ../include/object.h
    ../include/scheduler.h
    ../include/scoped_context.h
    ../include/scoped_string.h
    ../include/task.h
    ../include/util.h
    ../include/value.h
    ../include/globals/promise.h
    ../include/globals/console.h
    ../include/globals/context.h
    ../include/globals/filesystem.h
    ../include/globals/global.h
    ../include/globals/io.h
    ../include/globals/loader.h
    ../include/globals/module.h
    ../include/globals/net.h
    ../include/globals/scheduler.h
    ../include/classes/task.h
    ../include/classes/context.h
    ../include/classes/base.h
    ../include/classes/emitter.h
    ../include/classes/io/device.h
    ../include/classes/io/filter.h
    ../include/classes/io/stream.h
    ../include/classes/io/devices/file.h
    ../include/classes/io/devices/socket.h
    ../include/classes/io/filters/encoding.h
    ../include/classes/io/filters/utf8stringfilter.h
    ../include/classes/net/tcp/acceptor.h
    ../include/classes/net/htcommon/connection.h
    ../include/classes/net/htcommon/request.h
    ../include/classes/net/htcommon/response.h
    ../include/classes/net/http/connection.h
    ../include/classes/net/http/request.h
    ../include/classes/net/http/response.h
    ../include/classes/net/http/server.h
    ../include/classes/net/http2/server.h
    ../include/module.h)

add_executable(nexus ${INCLUDES}
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
    classes/net/http/request.cpp
    classes/net/http/response.cpp
    classes/net/http/connection.cpp
    classes/net/http2/server.cpp
    classes/net/htcommon/connection.cpp
    classes/net/htcommon/request.cpp
    classes/net/htcommon/response.cpp
    classes/context.cpp
    classes/emitter.cpp
    classes/task.cpp
    classes/base.cpp
    )

add_dependencies(nexus webkit-build)

target_link_libraries(nexus js_bundle JavaScriptCore WTF bmalloc ${Boost_LIBRARIES} ${ICU_LIBRARIES} ${ICU_I18N_LIBRARIES} pthread)
target_include_directories(nexus
    PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/generated/
    SYSTEM ${JAVASCRIPTCORE_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} ${ICU_INCLUDE_DIR} ${BEAST_INCLUDE_DIR})

link_directories(${Boost_LIBRARY_DIRS})

install(TARGETS nexus RUNTIME DESTINATION bin)
