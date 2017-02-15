cmake_minimum_required (VERSION 2.8)

project (libevpp)

set(DEBUG_MODE ON CACHE BOOL "Debug mode") 
set(LIB_INSTALL_DIR "/ust/local/lib/" CACHE PATH "")

if(NOT DEBUG_MODE)
  ##SET RELEASE MODE FLAGS
  SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
  SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
else()
  ##SET DEUBG MODE FLAGS
  SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS} -g" )
  SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS} -g" )
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(PROJECT_SOURCE_DIR src)
set(PROJECT_INCLUDE_DIR includes)

include_directories(${PROJECT_INCLUDE_DIR})
include_directories("/usr/include/")
include_directories("/usr/local/include")

link_directories("/usr/lib")
link_directories("/usr/local/lib")


add_library(event_loop
  ${PROJECT_SOURCE_DIR}/event_loop/timer_watcher.cpp
  ${PROJECT_SOURCE_DIR}/event_loop/socket_watcher.cpp
  ${PROJECT_SOURCE_DIR}/event_loop/event_loop_ev.cpp)

add_library(network
  ${PROJECT_SOURCE_DIR}/network/async_socket.cpp
  ${PROJECT_SOURCE_DIR}/network/unix_socket.cpp
  ${PROJECT_SOURCE_DIR}/network/tcp_socket.cpp)

## Compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-O2")        ## Optimize
    set(CMAKE_EXE_LINKER_FLAGS "-s")  ## Strip binary
endif()

target_link_libraries(network event_loop ev)

add_executable (a1.out test/main.cpp)
target_link_libraries(a1.out network)
