cmake_minimum_required(VERSION 2.8)

project(httpd-test-task)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -W -Wno-unused-parameter")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_COMPILER clang++)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread")
endif()

set(S_FILES
    backtrace.cc
    fd.cc
    http.cc
    misc.cc
    exception.cc
    poller.cc
    reactor.cc
    server.cc
    unix.cc
)

add_executable(httpd main.cc ${S_FILES})

add_custom_command(
    OUTPUT tests.cc
    COMMAND ./gen-tests.sh > tests.cc
    DEPENDS ./gen-tests.sh ${S_FILES}
)

add_executable(tests tests.cc ${S_FILES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(httpd dl)
    target_link_libraries(tests dl)
endif()

# vim: set ts=4 sw=4 et:
