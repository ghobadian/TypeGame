cmake_minimum_required(VERSION 3.22)
project(project C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(main)

add_executable(project
        main/helper_windows.h
        "main/New Text Document.c"
        main/shashi.h
        helper_windows.h
        main_windows.c
        shashi.h)
