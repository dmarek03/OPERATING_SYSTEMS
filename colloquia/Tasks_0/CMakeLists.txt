cmake_minimum_required(VERSION 3.22)
project(Tasks)

set(CMAKE_CXX_STANDARD 14)

add_executable(Tasks
        task1/main.c
        task1/Makefile
        Task2/main.c
        Task2/makefile
        task3/main.c
        task3/Makefile)
