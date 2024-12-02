cmake_minimum_required(VERSION 3.22)
project(zad2 C)

set(CMAKE_C_STANDARD 99)

add_executable(zad2  src/mode.h src/sender.c src/catcher.c)
