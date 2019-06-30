#include "elf.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <kprint.h>
static constexpr bool DEBUG_ELF = false;

bool Elf::validate(Elf64_Ehdr* hdr)
{
	return (hdr->e_ident[EI_MAG0] == ELFMAG0)
		&& (hdr->e_ident[EI_MAG1] == ELFMAG1)
		&& (hdr->e_ident[EI_MAG2] == ELFMAG2)
		&& (hdr->e_ident[EI_MAG3] == ELFMAG3)
		// we will need 64-bit addresses and such
		&& (hdr->e_ident[EI_CLASS] == ELFCLASS64);
}

template <typename T>
inline T* elf_offset(Elf64_Ehdr* hdr, intptr_t ofs) {
  return (T*) &((char*) hdr)[ofs];
}

static const Elf64_Sym*
elf_sym_index(Elf64_Ehdr* hdr, const Elf64_Shdr* shdr, uint32_t symidx)
{
	assert(symidx < shdr->sh_size / sizeof(Elf64_Sym));
	auto* symtab = elf_offset<Elf64_Sym>(hdr, shdr->sh_offset);
	return &symtab[symidx];
}

const Elf64_Shdr* Elf::section_by_name(Elf64_Ehdr* hdr, const char* name)
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

const Elf64_Sym* Elf::resolve_name(Elf64_Ehdr* hdr, const char* name)
{
	const auto* sym_hdr = Elf::section_by_name(hdr, ".symtab");
	assert(sym_hdr != nullptr);
	const auto* str_hdr = Elf::section_by_name(hdr, ".strtab");
	assert(str_hdr != nullptr);

	const Elf64_Sym* symtab = elf_sym_index(hdr, sym_hdr, 0);
	const size_t symtab_ents = sym_hdr->sh_size / sizeof(Elf64_Sym);
	const char* strtab = elf_offset<char>(hdr, str_hdr->sh_offset);

	for (size_t i = 0; i < symtab_ents; i++)
	{
		const char* symname = &strtab[symtab[i].st_name];
		//kprintf("Testing %s vs %s\n", symname, name);
		if (strcmp(symname, name) == 0) {
			return &symtab[i];
		}
	}
	return nullptr;
}

static void elf_print_sym(const Elf64_Sym* sym)
{
	kprintf("-> Sym is at %p with size %zu, type %u name %u\n",
			(void*) sym->st_value, sym->st_size,
			ELF64_ST_TYPE(sym->st_info), sym->st_name);
}

static void elf_relocate_section(Elf64_Ehdr* hdr, const char* section_name)
{
	const auto* rela = Elf::section_by_name(hdr, section_name);
	const auto* dyn_hdr = Elf::section_by_name(hdr, ".dynsym");
	const size_t rela_ents = rela->sh_size / sizeof(Elf64_Rela);
	auto* rela_addr = elf_offset<Elf64_Rela>(hdr, rela->sh_offset);
	for (size_t i = 0; i < rela_ents; i++) {
		const uint32_t symidx = ELF64_R_SYM(rela_addr[i].r_info);
		auto* sym = elf_sym_index(hdr, dyn_hdr, symidx);

		const uint8_t type = ELF64_ST_TYPE(sym->st_info);
		if (type == STT_FUNC || type == STT_OBJECT)
		{
			uintptr_t entry = (uintptr_t) hdr + rela_addr[i].r_offset;
			uintptr_t final = (uintptr_t) hdr + sym->st_value;
			if constexpr (DEBUG_ELF)
			{
				kprintf("Relocating rela %zu with sym idx %u where %p -> %p\n",
						i, symidx, (void*) entry, (void*) final);
				elf_print_sym(sym);
			}
			*(char**) entry = (char*) final;
		}
	}
}

void Elf::perform_relocations(Elf64_Ehdr* hdr)
{
	elf_relocate_section(hdr, ".rela.plt");
	elf_relocate_section(hdr, ".rela.dyn");
}
