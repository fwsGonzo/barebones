#include <main.h>
#include <kprint.h>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <elf.h>

extern char _binary_mylib_start;
extern char _binary_mylib_end;
static inline void validate_elf(Elf64_Ehdr* hdr);
static const Elf64_Sym* resolve_name(Elf64_Ehdr* hdr, const char* name);

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
	validate_elf(hdr);
	// translate function name into symbol
	const auto* sym = resolve_name(hdr, "init");
	assert(sym != nullptr);

	// call into library function
	const char* address = (char*) hdr + sym->st_value;
	int result = ((init_function_t) address)(&table);
	kprintf("init call result: %d\n", result);
}

template <typename T>
inline const T* elf_offset(Elf64_Ehdr* hdr, intptr_t ofs) {
  return (T*) &((char*) hdr)[ofs];
}
const Elf64_Sym* resolve_name(Elf64_Ehdr* hdr, const char* name)
{
	const auto* shdr = elf_offset<Elf64_Shdr> (hdr, hdr->e_shoff);
	const char* strtab = nullptr;
	const Elf64_Sym* symtab = nullptr;
	size_t symtab_size = 0;

	for (auto i = 0; i < hdr->e_shnum; i++)
	{
		switch (shdr[i].sh_type) {
		case SHT_SYMTAB:
			symtab = elf_offset<Elf64_Sym>(hdr, shdr[i].sh_offset);
			symtab_size = shdr[i].sh_size / sizeof(Elf64_Sym);
			break;
	    case SHT_STRTAB:
			// some flaky logic here, as we can only know its the right one
			// by checking its name using the first strtab :P
			if (symtab != nullptr && strtab == nullptr) {
				strtab = elf_offset<char>(hdr, shdr[i].sh_offset);
			}
			break;
		}
	}
	assert(strtab != nullptr);

	for (size_t i = 0; i < symtab_size; i++) {
		const char* symname = &strtab[symtab[i].st_name];
		//kprintf("Testing %s vs %s\n", symname, name);
		if (strcmp(symname, name) == 0) {
			return &symtab[i];
		}
	}

	return nullptr;
}

void validate_elf(Elf64_Ehdr* hdr) {
	assert(hdr->e_ident[EI_MAG0] == ELFMAG0);
    assert(hdr->e_ident[EI_MAG1] == ELFMAG1);
    assert(hdr->e_ident[EI_MAG2] == ELFMAG2);
  	assert(hdr->e_ident[EI_MAG3] == ELFMAG3);
	// we will need 64-bit code
	assert(hdr->e_ident[EI_CLASS] == ELFCLASS64);
}
