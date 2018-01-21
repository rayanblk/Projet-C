cmake_minimum_required(VERSION 3.9)
project(Projet_C C)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(Projet_C main.c callback.c mainFunction.c structure.h database.c)

target_link_libraries(Projet_C ${GTK3_LIBRARIES} libpq.dll)