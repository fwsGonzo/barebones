#include <stdint.h>
#include <assert.h>
#include "print.h"

void kernel_start(uint32_t eax, uint32_t ebx)
{
  kprintf("------------------\n");
  kprintf("Hello OSdev world!\n");

  kprintf("Multiboot EAX: 0x%x\n", eax);
  kprintf("Multiboot EBX: 0x%x\n", ebx);

  extern void test_cpp();
  test_cpp();

  kprint("Press Ctrl+A -> X to close\n");
}
