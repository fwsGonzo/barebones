#pragma once
#include <hw/serial.h>

// better, more familiar way to print!
extern int kprintf(const char* fmt, ...) __attribute__((format(printf, 1, 2)));

// print text directly to serial port
static inline void kprint(const char* text)
{
  __serial_print1(text);
}

#include <tinyprintf.h>
