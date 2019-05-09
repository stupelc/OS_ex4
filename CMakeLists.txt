cmake_minimum_required(VERSION 3.12)
project(OS_ex4_2 C)

set(CMAKE_C_STANDARD 99 -pthread)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(OS_ex4_2 main.c threadPool.c threadPool.h osqueue.c osqueue.h)