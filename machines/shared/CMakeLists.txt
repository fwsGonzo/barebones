cmake_minimum_required(VERSION 3.1)
project (kernel C CXX)
include(../../barebones.cmake)

add_machine_image(
	# name, binary and description
	mykernel "my_kernel" "This is a test kernel!"
	# list of source files
	main.cpp
)
#target_compile_definitions(mykernel PRIVATE MYTEST="4")

add_subdirectory(library)
add_machine_payload(mykernel mylib_payload
					${CMAKE_BINARY_DIR}/library/libmylib.so mylib)
add_dependencies(mylib_payload mylib)
