cmake_minimum_required (VERSION 2.6)
project (SOCKET)

set (SOCKET_VERSION_MAJOR 1)
set (SOCKET_VERSION_MINOR 0)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/inc/tcp_1_common.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/inc/tcp_1_common.h"
    )

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/inc/")
add_executable (tcp_server_basic ${CMAKE_CURRENT_SOURCE_DIR}/src/server_tcp_1.c)
add_executable (tcp_client_basic ${CMAKE_CURRENT_SOURCE_DIR}/src/client_tcp_1.c)
