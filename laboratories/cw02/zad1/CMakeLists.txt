cmake_minimum_required(VERSION 3.22)
project(lab_02 C)

set(CMAKE_C_STANDARD 99)

add_executable(lab_02 collatz.h collatz.c client.c client.c)
