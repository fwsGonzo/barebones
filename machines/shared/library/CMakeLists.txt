cmake_minimum_required(VERSION 3.1)
project(mylib VERSION 1.0 DESCRIPTION "mylib description")

add_library(mylib SHARED
		library.cpp
	)

target_compile_options(mylib PRIVATE "-nostdlib")

set_target_properties(mylib PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(mylib PROPERTIES SOVERSION 1)

set_target_properties(mylib PROPERTIES PUBLIC_HEADER library.hpp)
target_include_directories(mylib PRIVATE .)
