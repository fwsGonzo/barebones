#include <stdint.h>

static const uint16_t port = 0x3F8; // Serial 1

static inline uint8_t inb(int port)
{
  int ret;
  asm volatile ("xorl %eax,%eax");
  asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));
  return ret;
}
static inline void outb(int port, uint8_t data)
{
  asm("outb %%al,%%dx"::"a" (data), "d"(port));
}

void __init_serial1()
{
  // properly initialize serial port
  outb(port + 1, 0x00);    // Disable all interrupts
  outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  outb(port + 1, 0x00);    //                  (hi byte)
  outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
  outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
}

void __serial_print1(const char* cstr)
{
  while (*cstr) {
    while (!(inb(port + 5) & 0x20));
    outb(port, *cstr++);
  }
}
void __serial_print(const char* str, int len)
{
  for (int i = 0; i < len; i++) {
    while (!(inb(port + 5) & 0x20));
    outb(port, str[i]);
  }
}
