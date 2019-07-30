#include <stdint.h>
asm(".org 0x2000");

void hotswap(const char* base, int len, char* dest, void* start,
             uintptr_t magic, uintptr_t bootinfo)
{
  for (int i = 0; i < len; i++)
    dest[i] = base[i];

  asm volatile("jmp *%0" : : "r" (start), "a" (magic), "b" (bootinfo));
  asm volatile (".global __hotswap_end;\n__hotswap_end:");
  __builtin_unreachable();
}
