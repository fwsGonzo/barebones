#pragma once
#include <kernel/elf.hpp>

struct Dylib
{
	static Elf64_Ehdr* load(const void* address);

	template <typename T>
	static T resolve_function(const Elf64_Ehdr* hdr, const char* name);
};


inline Elf64_Ehdr* Dylib::load(const void* address)
{
	auto* hdr = (Elf64_Ehdr*) address;
	// validate shared library ELF header
	assert(Elf::validate(hdr));
	// resolve symbolic references from PLT and GOT
	Elf::perform_relocations(hdr);
	return hdr;
}
template <typename T>
inline T Dylib::resolve_function(const Elf64_Ehdr* hdr, const char* name)
{
	const auto* sym = Elf::resolve_name(hdr, name);
	if (sym == nullptr) return T();
	return (T) &((char*) hdr)[sym->st_value];
}
