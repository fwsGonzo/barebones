#include <stdint.h>

extern uint32_t multiboot_data_magic;
extern uint32_t multiboot_data_address;
extern void kernel_start(uint32_t eax, uint32_t ebx);

void begin_enter_longmode()
{
	kernel_start(multiboot_data_magic, multiboot_data_address);
}
