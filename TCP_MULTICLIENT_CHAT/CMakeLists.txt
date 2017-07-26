cmake_minimum_required (VERSION 2.6)
project (SOCKET)

set (SOCKET_VERSION_MAJOR 1)
set (SOCKET_VERSION_MINOR 0)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/inc/tcp_2_common.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/inc/tcp_2_common.h"
    )

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/inc/")

#build client list lib
add_subdirectory (src/TCP_CLIENT_LIST)

#build server and client
add_executable (tcp_server_chat ${CMAKE_CURRENT_SOURCE_DIR}/src/server_tcp_2_chat.c)
add_executable (tcp_client_chat ${CMAKE_CURRENT_SOURCE_DIR}/src/client_tcp_2_chat.c)

#link server with client list lib
target_link_libraries (tcp_server_chat TcpClientList)
