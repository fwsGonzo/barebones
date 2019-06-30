#include <main.h>
#include <kprint.h>
#include <cassert>
#include <cstring>
#include <cstdlib>

extern char _binary_mylib_start;
extern char _binary_mylib_end;
#include <kernel/dylib.hpp>
#include "library/library.hpp"

void kernel_main(const uint32_t, const uint32_t)
{
	// load address is the start of the mylib blob
	auto* hdr = Dylib::load(&_binary_mylib_start);
	assert(hdr != nullptr);

	// translate init function name into symbol and create callable function
	auto init = Dylib::resolve_function<init_function_t>(hdr, "init");
	assert(init != nullptr);

	// setup call table
	static jumptable table;
	table.kprintf = kprintf;
	table.malloc = malloc;
	table.free = free;

	// call into library function
	int result = init(&table);
	kprintf("init call result: %d\n", result);
}
