#include <assert.h>
#include <stdint.h>
#include <kprint.h>

void kernel_start(uint32_t eax, uint32_t ebx)
{
  assert(eax == 0x2badb002);

  extern void __init_stdlib(uint32_t, uint32_t);
  __init_stdlib(eax, ebx);

  extern void main(uint32_t, uint32_t);
  main(eax, ebx);
}
