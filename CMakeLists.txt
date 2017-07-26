cmake_minimum_required (VERSION 2.6)
project (SOCKET)

set (SOCKET_VERSION_MAJOR 1)
set (SOCKET_VERSION_MINOR 0)

configure_file (
    "${PROJECT_SOURCE_DIR}/showip.h.in"
    "${PROJECT_SOURCE_DIR}/showip.h"
    )

include_directories ("${PROJECT_SOURCE_DIR}")



add_executable (SHOW_IP showip.c)
