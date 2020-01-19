#include <assert.h>
#include <kprint.h>
#include <stddef.h>
#include <stdint.h>
#include <multiboot.h>

extern size_t strlen(const char* str);
extern char _end;

static void* multiboot_free_begin(intptr_t mb_addr)
{
	const multiboot_info_t* info = (multiboot_info_t*) mb_addr;
	uintptr_t end = (uintptr_t) &_end;

	if (info->flags & MULTIBOOT_INFO_CMDLINE) {
		const char* cmdline = (char*) (intptr_t) info->cmdline;
		const uintptr_t pot_end = info->cmdline + strlen(cmdline);
		if (pot_end > end) end = pot_end;
	}

	const multiboot_module_t* mod = (multiboot_module_t*) (intptr_t) info->mods_addr;
	const multiboot_module_t* mods_end = mod + info->mods_count;

	for (; mod < mods_end; mod++)
	{
		if (mod->mod_end > end) end = mod->mod_end;
	}
	return (void*) end;
}

void __init_stdlib(uint32_t mb_magic, uint32_t mb_addr)
{
	assert(mb_magic == 0x2badb002);

	// 1. enable printf facilities
	init_printf(NULL, __serial_putchr);

	// 2. find end of multiboot areas
	void* free_begin = multiboot_free_begin(mb_addr);
	assert(free_begin >= (void*) &_end);

	// 3. initialize heap (malloc, etc.)
	extern void __init_heap(void*);
	__init_heap(free_begin);

#ifdef EH_ENABLED
	/// 4. initialize exceptions before we run constructors
	extern char __eh_frame_start[];
	extern void __register_frame(void*);
	__register_frame(&__eh_frame_start);
#endif

	// 5. call global C/C++ constructors
	extern void(*__init_array_start [])();
	extern void(*__init_array_end [])();
	int count = __init_array_end - __init_array_start;
	for (int i = 0; i < count; i++) {
		__init_array_start[i]();
	}
}
