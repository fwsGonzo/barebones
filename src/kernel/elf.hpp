#pragma once
#include <elf.h>

struct Elf
{
	static bool validate(Elf64_Ehdr* hdr);
	static const Elf64_Shdr* section_by_name(Elf64_Ehdr*, const char* name);

	// get the symbol associated with @name
	static const Elf64_Sym* resolve_name(Elf64_Ehdr*, const char* name);
	// dynamic loader
	static void perform_relocations(Elf64_Ehdr* hdr);
};
