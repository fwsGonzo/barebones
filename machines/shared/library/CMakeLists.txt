cmake_minimum_required(VERSION 3.1)
project(mylib VERSION 1.0 DESCRIPTION "mylib description")

add_library(mylib SHARED
		library.cpp
	)

# unfortunately we have to do this to shake some of the arguments from the
# top level project
set(CMAKE_CXX_FLAGS "-Wall -Wextra -g -O2 -ffreestanding -fno-omit-frame-pointer")

target_compile_options(mylib PRIVATE "-nostdlib")
target_compile_options(mylib PRIVATE "-Os")

set_target_properties(mylib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(mylib PROPERTIES SOVERSION 1)

set_target_properties(mylib PROPERTIES PUBLIC_HEADER library.hpp)
target_include_directories(mylib PRIVATE .)
