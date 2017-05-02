cmake_minimum_required(VERSION 3.7)
project(debugger)

set(CMAKE_C_STANDARD 11)

set(SOURCE_FILES main.c memory_access.c memory_access.h vector.c vector.h memory_search.c memory_search.h)
add_executable(debugger ${SOURCE_FILES})
add_compile_options(-Wall -Wextra -Wpedantic)