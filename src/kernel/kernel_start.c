#include <assert.h>
#include <stdint.h>
#include <kprint.h>
#include "tls.h"

void kernel_start(uint32_t eax, uint32_t ebx)
{
  kprintf("kernel_start(eax: %x, ebx: %x)\n", eax, ebx);
  assert(eax == 0x2badb002);

  extern void __init_stdlib(uint32_t, uint32_t);
  __init_stdlib(eax, ebx);

  extern void kernel_main(uint32_t, uint32_t);
  kernel_main(eax, ebx);
}
