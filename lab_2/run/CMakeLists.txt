cmake_minimum_required(VERSION 3.16)

project(run VERSION 1.0)


include_directories(run ../inc)
add_executable(run run.c) 

target_link_libraries(run multi_protocol_server_client)
