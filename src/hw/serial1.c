#include <stdint.h>

static const uint16_t port = 0x3F8; // Serial port 1
static char initialized __attribute__((section(".data"))) = 0;

static inline uint8_t inb(int port)
{
	int ret;
	asm ("xorl %eax,%eax");
	asm ("inb %%dx,%%al":"=a" (ret):"d"(port));
	return ret;
}
static inline void outb(int port, uint8_t data)
{
	asm ("outb %%al,%%dx"::"a" (data), "d"(port));
}

static inline void init_serial_if_needed()
{
	if (initialized) return;
	initialized = 1;
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
	init_serial_if_needed();
	while (*cstr) {
		while (!(inb(port + 5) & 0x20)) /* */;
		outb(port, *cstr++);
	}
}
void __serial_print(const char* str, int len)
{
	init_serial_if_needed();
	for (int i = 0; i < len; i++) {
		while (!(inb(port + 5) & 0x20)) /* */;
		outb(port, str[i]);
	}
}
void __serial_putchr(const void* file, char c)
{
	static char buffer[256];
	static unsigned cnt = 0;
	(void) file;
	buffer[cnt++] = c;
	if (c == '\n' || cnt == sizeof(buffer)) {
		__serial_print(buffer, cnt);
		cnt = 0;
	}
}
