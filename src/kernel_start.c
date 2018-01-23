#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
extern void __serial_print1(const char*);

void print_hex32(uint32_t val)
{
  const char lut[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                      '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  char buffer[9] = {'F', 'F', 'F', 'F', 'F', 'F', 'F', 'F', 0};
  for (int i = 0; i < 4; i++)
  {
    buffer[i*2+0] = lut[(val >> (28-i*8)) & 0xF];
    buffer[i*2+1] = lut[(val >> (24-i*8)) & 0xF];
  }
  __serial_print1(buffer);
}

void kernel_start(uint32_t eax, uint32_t ebx)
{
  __serial_print1("Hello OSdev world!\n");

  __serial_print1("Multiboot EAX: 0x");
      print_hex32(eax); __serial_print1("\n");
  __serial_print1("Multiboot EBX: 0x");
      print_hex32(ebx); __serial_print1("\n");

  extern void test_cpp();
  test_cpp();

  __serial_print1("Press Ctrl+A -> X to close\n");
}
