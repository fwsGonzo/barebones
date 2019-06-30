#pragma once
#include <cstddef>

struct jumptable
{
	int   (*kprintf)(const char* string, ...);
	void* (*malloc) (size_t);
	void  (*free)   (void*);
};

typedef int (*init_function_t) (jumptable*);
