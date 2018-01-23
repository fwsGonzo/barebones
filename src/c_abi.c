#include <stdint.h>
extern void __serial_print1(const char*);
__attribute__((noreturn)) void panic(const char*);

void __init_stdlib()
{
  extern char _BSS_START_;
  extern char _BSS_END_;

  for (char* bss = &_BSS_START_; bss < &_BSS_END_; bss++) {
    *bss = 0;
  }
}

void __stack_chk_fail_local()
{
  panic("Stack protector: Canary modified");
  __builtin_unreachable();
}

void panic(const char* why)
{
  __serial_print1("\n\n!!! PANIC !!!\n");
  __serial_print1(why);

  // the end
  __serial_print1("\n");
  while(1) asm("cli; hlt");
  __builtin_unreachable();
}
