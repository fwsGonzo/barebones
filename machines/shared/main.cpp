#include <main.h>
#include <kprint.h>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <kernel/elf.hpp>

extern char _binary_mylib_start;
extern char _binary_mylib_end;

#include "library/library.hpp"
static jumptable table;

void kernel_main(const uint32_t, const uint32_t)
{
	// setup call table
	table.kprintf = kprintf;
	table.malloc = malloc;
	table.free = free;

	auto* hdr = (Elf64_Ehdr*) &_binary_mylib_start;
	// validate shared library ELF header
	assert(Elf::validate(hdr));
	// resolve symbolic references from PLT and GOT
	Elf::perform_relocations(hdr);
	// translate init function name into symbol
	const auto* sym = Elf::resolve_name(hdr, "init");
	assert(sym != nullptr);
	// create callable init function
	auto library_init = (init_function_t) &((char*) hdr)[sym->st_value];
	// call into library function
	int result = library_init(&table);
	kprintf("init call result: %d\n", result);
}
