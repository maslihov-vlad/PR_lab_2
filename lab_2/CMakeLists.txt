cmake_minimum_required(VERSION 3.16)

project(multi_protocol_server_client VERSION 1.0)

include_directories(multi_protocol_server_client inc)
add_library(multi_protocol_server_client src/server.c) 

add_subdirectory(run) 

