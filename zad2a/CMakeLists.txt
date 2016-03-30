cmake_minimum_required(VERSION 3.4)
project(zad2 C)

set(CMAKE_C_FLAGS "-Wall")

add_library(main_lib main_lib.c)

add_executable(sender sender.c)
add_executable(catcher catcher.c)

target_link_libraries(sender main_lib)
target_link_libraries(catcher main_lib)
