cmake_minimum_required(VERSION 2.8.9)
project (kernel C CXX)

option(NATIVE          "Compile natively for this CPU" OFF)
option(MINIMAL         "Compile small executable" OFF)
option(EASTL           "Compile with EASTL C++ library" OFF)
option(LTO_ENABLE      "Enable LTO (Clang-only)" OFF)
option(STACK_PROTECTOR "Enable stack protector (SSP)" ON)
option(UBSAN           "Enable the undefined sanitizer" OFF)
option(STRIPPED        "Strip the executable" OFF)
option(DEBUG           "Build and preserve debugging information" OFF)
set(CPP_VERSION "c++17" CACHE STRING "C++ version compiler argument")
set(C_VERSION   "gnu11" CACHE STRING "C version compiler argument")
set(LINKER_EXE  "ld"    CACHE STRING "Linker to use")

set(KERNEL_DESC  "This is a test kernel!")
set(BINARY_NAME  "my_kernel")
set(SOURCES
	# .c files
	src/kernel/kernel_start.c
	src/hw/serial1.c
	src/crt/c_abi.c
	src/crt/heap.c
	src/crt/malloc.c
	src/crt/ubsan.c
	src/prnt/print.c
	src/prnt/mini-printf.c
	# .cpp files
	src/main.cpp
	src/crt/cxxabi.cpp
	src/kernel/tls.cpp src/kernel/panic.cpp
	# .asm files (for NASM)
	src/kernel/start.asm
	src/kernel/start64.asm
  )

enable_language(ASM_NASM)
set(ELF_FORMAT "x86_64")
set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf64")
set(OBJCOPY_TARGET "elf64-x86-64")
set(CAPABS "-Wall -Wextra -g -m64 -ffreestanding -fno-omit-frame-pointer")

# Optimization flags
set(OPTIMIZE "-mfpmath=sse -msse3")
if (NATIVE)
	set(OPTIMIZE "${OPTIMIZE} -Ofast -march=native")
elseif (MINIMAL)
	set(OPTIMIZE "${OPTIMIZE} -Oz")
endif()

set(CMAKE_CXX_FLAGS "-MMD ${CAPABS} ${OPTIMIZE} -std=${CPP_VERSION}")
set(CMAKE_C_FLAGS "-MMD ${CAPABS} ${OPTIMIZE} -std=${C_VERSION}")
if (LTO_ENABLE)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto=thin")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto=thin")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flto")
	endif()
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti")

# Sanitizer options
if (UBSAN)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fno-sanitize=vptr")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined -fno-sanitize=vptr")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sanitize=function")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-sanitize=function")
	endif()
endif()
if (STACK_PROTECTOR)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-strong")
endif()

add_executable(kernel ${SOURCES})
set_target_properties(kernel PROPERTIES OUTPUT_NAME ${BINARY_NAME})
target_include_directories(kernel PRIVATE src)

target_compile_definitions(kernel PRIVATE KERNEL_BINARY="${BINARY_NAME}")
target_compile_definitions(kernel PRIVATE KERNEL_DESC="${KERNEL_DESC}")

add_subdirectory(ext)
if (EASTL)
	target_link_libraries(kernel eastl)
endif()

# linker stuff
set(CMAKE_LINKER ${LINKER_EXE})
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS) # this removed -rdynamic from linker output
set(BUILD_SHARED_LIBRARIES OFF)
set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> -o <TARGET> <LINK_FLAGS> <OBJECTS> <LINK_LIBRARIES>")

string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef SSP_VALUE)
# Add --eh-frame-hdr for exception tables
set(LDSCRIPT "${CMAKE_SOURCE_DIR}/src/linker.ld")
set(LDFLAGS "-static -nostdlib -N -melf_${ELF_FORMAT} --script=${LDSCRIPT} --defsym __SSP__=0x${SSP_VALUE}")
if (NOT DEBUG AND STRIPPED)
	set(LDFLAGS "${LDFLAGS} -s")
elseif (NOT DEBUG)
	set(LDFLAGS "${LDFLAGS} -S")
endif()
set_target_properties(kernel PROPERTIES LINK_FLAGS "${LDFLAGS}")

# write out the binary name to a known file to simplify some scripts
file(WRITE ${CMAKE_BINARY_DIR}/binary.txt ${BINARY_NAME})
