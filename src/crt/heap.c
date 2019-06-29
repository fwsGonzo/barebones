#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static uintptr_t heap_start;
static uintptr_t heap_end;
// heap_max really should be set to max physical
// by reading the value from multiboot meminfo table
static uintptr_t heap_max = 0xC0000000;

void __init_heap(void* free_begin)
{
	heap_start = (uintptr_t) free_begin;
	if (heap_start & 0xF) heap_start += 0x10 - (heap_start & 0xF);
	heap_end   = heap_start;
	assert(((heap_start & 0xF) == 0) && "Heap should be aligned");
}

// data segment size
void* sbrk(intptr_t increment)
{
	uintptr_t old = heap_end;
	if (heap_end + increment <= heap_max)
	{
		heap_end += increment;
		return (void*) old;
	}
	return (void*) -1;
}

int posix_memalign(void **memptr, size_t alignment, size_t size)
{
	uintptr_t addr = (uintptr_t) malloc(size + alignment);
	const intptr_t offset = addr & (alignment-1);
	if (offset) addr += (alignment-1) - offset;
	*memptr = (void*) addr;
	return 0;
}
