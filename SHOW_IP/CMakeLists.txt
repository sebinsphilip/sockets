cmake_minimum_required (VERSION 2.6)
project (SOCKET)

set (SOCKET_VERSION_MAJOR 1)
set (SOCKET_VERSION_MINOR 0)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/inc/showip.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/inc/showip.h"
    )

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/inc/")
message ("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}:")
message ("PROJECT_SOURCE: ${PROJECT_SOURCE_DIR}:")
message ("PROJECT_BINARY: ${PROJECT_BINARY_DIR}:")


add_executable (show_host_ip ${CMAKE_CURRENT_SOURCE_DIR}/src/showip.c)
