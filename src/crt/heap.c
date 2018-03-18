#include <stdint.h>
#include <assert.h>

uintptr_t heap_start;
uintptr_t heap_end;
// heap_max really should be set to max physical
// by reading the value from multiboot meminfo table
uintptr_t heap_max = 0xC0000000;

void __init_heap(void* free_begin)
{
  heap_start = (uintptr_t) free_begin;
  heap_end   = (uintptr_t) free_begin;
  assert((heap_start & 0x7) == 0 && "Heap should be aligned");
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
