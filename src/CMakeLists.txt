
set(KERNEL_SOURCES
	# .c files
	kernel/kernel_start.c
	hw/serial1.c
	crt/c_abi.c
	crt/c_stubs.c
	crt/heap.c
	crt/malloc.c
	crt/ubsan.c
	prnt/print.c
	# .cpp files
	crt/cxxabi.cpp
	kernel/tls.cpp
	kernel/panic.cpp
	# .asm files (for NASM)
	kernel/start.asm
	kernel/start64.asm
  )

add_library(kernel STATIC ${KERNEL_SOURCES})
target_include_directories(kernel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(kernel tinyprintf)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set_source_files_properties(
			kernel/panic.cpp
		PROPERTIES COMPILE_FLAGS -Wno-frame-address)
endif()

if (RTTI_EXCEPTIONS)
	target_compile_definitions(kernel PRIVATE -DEH_ENABLED)
endif()
