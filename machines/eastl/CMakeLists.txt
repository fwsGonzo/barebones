cmake_minimum_required(VERSION 3.1)
project (kernel C CXX)

option(EASTL "" ON)
option(RTTI_EXCEPTIONS "" ON)
include(../../barebones.cmake)

add_machine_image(
	# name, binary and description
	mykernel "eastl_kernel" "This is a test for EASTL!"
	# list of source files
	main.cpp
)
