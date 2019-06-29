#define _GNU_SOURCE
#include <stddef.h>
#include <dlfcn.h>
#include <kprint.h>

char *getenv(const char *name) {
	//kprintf("getenv called for %s\n", name);
	(void) name;
	return NULL;
}
int setenv(const char *name, const char *value, int overwrite) {
	(void) name;
	(void) value;
	(void) overwrite;
	return -1;
}
