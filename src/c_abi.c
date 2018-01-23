#include <stdint.h>
extern void __serial_print1(const char*);
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
  __serial_print1("Assertion failed: ");
      __serial_print1(assertion);
  __serial_print1("  in file ");
      __serial_print1(file);
  __serial_print1(", function ");
      __serial_print1(function);
  __serial_print1("\n");
  panic(assertion);
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
