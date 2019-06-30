#include "library.hpp"

extern "C"
int init(jumptable* t)
{
	jumptable* table = t;
	table->kprintf("Hello world from a shared library! table = %p\n", table);

	char* data = (char*) table->malloc(4096);
	table->kprintf("malloc() returned %p\n", data);

	for (int i = 0; i < 4096; i++) {
		data[i] = i & 0xFF;
	}
	table->free(data);

	return 0;
}
