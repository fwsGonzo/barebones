#include <stdint.h>
#include "print.h"
__attribute__((noreturn)) void panic(const char*);

void __init_stdlib()
{
  // initialize BSS area
  extern char _BSS_START_;
  extern char _BSS_END_;

  for (char* bss = &_BSS_START_; bss < &_BSS_END_; bss++) {
    *bss = 0;
  }

  // call global C/C++ constructors
  extern void(*__init_array_start [])();
  extern void(*__init_array_end [])();
  int count = __init_array_end - __init_array_start;
  for (int i = 0; i < count; i++)
      __init_array_start[i]();
}

void __stack_chk_fail_local()
{
  panic("Stack protector: Canary modified");
  __builtin_unreachable();
}
void __stack_chk_fail()
{
  panic("Stack protector: Canary modified");
  __builtin_unreachable();
}
void __assert_fail(const char *assertion,
                    const char *file,
                    unsigned int line,
                    const char *function)
{
  char buffer[4096];
  snprintf(buffer, sizeof(buffer),
          "Assertion failed: %s in %s:%u, function %s\n",
          assertion, file, line, function);
  panic(buffer);
}

void panic(const char* why)
{
  kprintf("\n\n!!! PANIC !!!\n%s\n", why);

  // the end
  while(1) __asm__ ("cli; hlt");
  __builtin_unreachable();
}
