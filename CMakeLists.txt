cmake_minimum_required(VERSION 3.20)
project(Chat_Application)

set(CMAKE_CXX_STANDARD 17)

add_executable(Chat_Server
        server.cpp)
add_executable(Chat_Client
        client.cpp)