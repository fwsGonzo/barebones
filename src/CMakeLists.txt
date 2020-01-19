
set(KERNEL_SOURCES
	# .c files
	kernel/kernel_start.c
	kernel/init_libc.c
	hw/serial1.c
	crt/c_abi.c
	crt/c_stubs.c
	crt/heap.c
	crt/malloc.c
	crt/print.c
	crt/ubsan.c
	# .cpp files
	crt/cxxabi.cpp
	kernel/elf.cpp
	kernel/tls.cpp
	kernel/panic.cpp
	# .asm files (for NASM)
	kernel/start.asm
  )
if (BUILD_32)
	list(APPEND KERNEL_SOURCES
	  	kernel/start32.c
		crt/udiv.c
	)
else()
	list(APPEND KERNEL_SOURCES
  		kernel/start64.asm
	)
endif()

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
