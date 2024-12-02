cmake_minimum_required(VERSION 3.22)
project(Tasks)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Task1)
include_directories(Task2)
include_directories(Task3)

add_executable(Tasks
        Task1/main.c
        Task1/Makefile
        Task2/main.c
        Task2/Makefile
        Task3/file1
        Task3/file2
        Task3/main.c
        Task3/Makefile)
