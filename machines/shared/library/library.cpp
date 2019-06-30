#include "interface.hpp"
static int test_value = 666;

struct Test {
	int a;
};
Test test;

extern "C"
__attribute__((noinline))
void test_function(jumptable* table)
{
	table->kprintf("Hello from test()! jumptable = %p\n", table);
}

extern "C"
int init(jumptable* t)
{
	jumptable* table = t;
	table->kprintf("Hello world from a shared library! table = %p\n", table);
	table->kprintf("test_value == %d\n", test_value);

	char* data = (char*) table->malloc(4096);
	table->kprintf("malloc() returned %p\n", data);

	for (int i = 0; i < 4096; i++) {
		data[i] = i & 0xFF;
	}
	table->free(data);

	test.a = 55;
	test_function(t);
	return 0;
}
