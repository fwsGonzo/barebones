cmake_minimum_required(VERSION 2.8.9)
project (kernel C CXX)
include(../../barebones.cmake)

add_machine_image(
	# name, binary and description
	mykernel "my_kernel" "This is a test kernel!"
	# list of source files
	main.cpp
)

target_compile_definitions(mykernel PRIVATE MYTEST="4")
