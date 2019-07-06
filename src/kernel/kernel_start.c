#include <assert.h>
#include <stdint.h>
#include <kprint.h>

static void __init_paging()
{
	static uintptr_t pdir0[512] __attribute__((aligned(4096)));
	// unmap zero page
	assert(((uintptr_t) pdir0 & 0xfff) == 0);
	pdir0[0] = 0x0; // unpresent zero page
	for (uintptr_t i = 1; i < 512; i++) {
		pdir0[i] = (i * 0x1000) | 0x3; // RW + P
	}
	// install into PML2 entry 0
	uintptr_t* pml2 = (uintptr_t*) 0x3000;
	pml2[0] = ((uintptr_t) pdir0) | 0x3; // RW + P;
	__asm__ ("invlpg 0x0");
}

void kernel_start(uint32_t eax, uint32_t ebx)
{
	kprintf("kernel_start(eax: %x, ebx: %x)\n", eax, ebx);

	extern void __init_stdlib(uint32_t, uint32_t);
	__init_stdlib(eax, ebx);

	// we have to do this after initializing .bss
	// NOTE: don't enable this until you catch CPU exceptions!
	//__init_paging();

	extern void kernel_main(uint32_t, uint32_t);
	kernel_main(eax, ebx);
}
