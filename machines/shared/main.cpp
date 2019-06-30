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
static const Elf64_Shdr* section_by_name(Elf64_Ehdr* hdr, const char* name);
static void elf_perform_relocations(Elf64_Ehdr* hdr);

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
	// resolve symbolic references from PLT and GOT
	elf_perform_relocations(hdr);
	// translate init function name into symbol
	const auto* sym = resolve_name(hdr, "init");
	assert(sym != nullptr);
	// create callable init function
	auto library_init = (init_function_t) &((char*) hdr)[sym->st_value];
	// call into library function
	int result = library_init(&table);
	kprintf("init call result: %d\n", result);
}

template <typename T>
inline T* elf_offset(Elf64_Ehdr* hdr, intptr_t ofs) {
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

const Elf64_Shdr* section_by_name(Elf64_Ehdr* hdr, const char* name)
{
	const auto* shdr = elf_offset<Elf64_Shdr> (hdr, hdr->e_shoff);
	const auto& shstrtab = shdr[hdr->e_shnum-1];
	const char* strings = elf_offset<char>(hdr, shstrtab.sh_offset);

	for (auto i = 0; i < hdr->e_shnum; i++)
	{
		const char* shname = &strings[shdr[i].sh_name];
		if (strcmp(shname, name) == 0) {
			return &shdr[i];
		}
	}
	return nullptr;
}

static const Elf64_Sym*
resolve_dyn_idx(Elf64_Ehdr* hdr, const Elf64_Shdr* shdr, uint32_t symidx)
{
	assert(symidx < shdr->sh_size / sizeof(Elf64_Sym));
	auto* symtab = elf_offset<Elf64_Sym>(hdr, shdr->sh_offset);
	return &symtab[symidx];
}

static void elf_print_sym(const Elf64_Sym* sym)
{
	kprintf("-> Sym is at %p with size %zu, type %u name %u\n",
			(void*) sym->st_value, sym->st_size,
			ELF64_ST_TYPE(sym->st_info), sym->st_name);
}

static void elf_relocate_section(Elf64_Ehdr* hdr, const char* section_name)
{
	const auto* rela = section_by_name(hdr, section_name);
	const auto* dyn_hdr = section_by_name(hdr, ".dynsym");
	const size_t rela_ents = rela->sh_size / sizeof(Elf64_Rela);
	auto* rela_addr = elf_offset<Elf64_Rela>(hdr, rela->sh_offset);
	for (size_t i = 0; i < rela_ents; i++) {
		const uint32_t symidx = ELF64_R_SYM(rela_addr[i].r_info);
		auto* sym = resolve_dyn_idx(hdr, dyn_hdr, symidx);

		const uint8_t type = ELF64_ST_TYPE(sym->st_info);
		if (type == STT_FUNC || type == STT_OBJECT)
		{
			uintptr_t entry = (uintptr_t) hdr + rela_addr[i].r_offset;
			uintptr_t final = (uintptr_t) hdr + sym->st_value;
			//kprintf("Relocating rela %zu with sym idx %u where %p -> %p\n",
			//		i, symidx, (void*) entry, (void*) final);
			//elf_print_sym(sym);
			*(char**) entry = (char*) final;
		}
	}
}

void elf_perform_relocations(Elf64_Ehdr* hdr)
{
	elf_relocate_section(hdr, ".rela.plt");
	elf_relocate_section(hdr, ".rela.dyn");
}

void validate_elf(Elf64_Ehdr* hdr) {
	assert(hdr->e_ident[EI_MAG0] == ELFMAG0);
    assert(hdr->e_ident[EI_MAG1] == ELFMAG1);
    assert(hdr->e_ident[EI_MAG2] == ELFMAG2);
  	assert(hdr->e_ident[EI_MAG3] == ELFMAG3);
	// we will need 64-bit code
	assert(hdr->e_ident[EI_CLASS] == ELFCLASS64);
}
