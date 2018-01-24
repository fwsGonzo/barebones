#include "prnt/mini-printf.h"
#include "serial.h"

// better, more familiar way to print!
extern int kprintf(const char* fmt, ...);

// print text directly to serial port
static inline void kprint(const char* text)
{
  __serial_print1(text);
}
