cmake_minimum_required(VERSION 3.1)
project (kernel C CXX)

option(BUILD_32 "" ON)
include(../../barebones.cmake)

add_machine_image(
	# name, binary and description
	mykernel "my_kernel" "This is a 32-bit test kernel!"
	# list of source files
	main.cpp
)

target_compile_definitions(mykernel PRIVATE MYTEST="4")
