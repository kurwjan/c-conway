cmake_minimum_required(VERSION 3.27)
project(conway C)

set(CMAKE_C_STANDARD 23)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(conway conway.c
        conway.h)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})
target_link_libraries(conway ${CURSES_LIBRARIES})