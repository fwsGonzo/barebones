#define _GNU_SOURCE
#include <stddef.h>
#include <dlfcn.h>
#include <link.h>
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
int dl_iterate_phdr(
	int (*callback) (struct dl_phdr_info *info, size_t size, void *data), void *data)
{
	return 0;
}
