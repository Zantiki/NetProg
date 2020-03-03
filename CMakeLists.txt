cmake_minimum_required(VERSION 3.0)
project(NetProg)

set(CMAKE_CXX_STANDARD 14)

#add_executable(NetProg WorkerThreads.cpp)
add_executable(server WIN32 Server_and_client/server.cpp)
add_executable(klient Server_and_client/klient.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -pthread -std=c++1y -Wall -Wextra")
