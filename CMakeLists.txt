cmake_minimum_required(VERSION 3.10)
project(NetProg)

set(CMAKE_CXX_STANDARD 14)

add_executable(NetProg PrimeThread.cpp WorkerThreads.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -pthread -std=c++1y -Wall -Wextra")