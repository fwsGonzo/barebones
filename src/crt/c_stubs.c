#define _GNU_SOURCE
#include <dlfcn.h>
#include <pthread.h>
#include <kprint.h>
#include <link.h>
struct link_map *_dl_loaded;

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

int dladdr(const void *addr, Dl_info *info) {
	kprintf("dl_addr called\n");
	(void) addr;
	(void) info;
	return -1;
}
int dl_iterate_phdr(int (*callback) (struct dl_phdr_info *info,
					size_t size, void *data), void *data)
{
	kprintf("dl_iterate_phdr called\n");
	int ret = 0;
	/*
	struct link_map *l;
	struct dl_phdr_info info;

	for (l = _dl_loaded; l != NULL; l = l->l_next)
	{
		// Skip the dynamic linker.
		if (l->l_phdr == NULL)
			continue;
		info.dlpi_addr = l->l_addr;
		info.dlpi_name = l->l_name;
		info.dlpi_phdr = l->l_phdr;
		info.dlpi_phnum = l->l_phnum;
		ret = callback (&info, sizeof (struct dl_phdr_info), data);
		if (ret)
		break;
    }
	*/
	return ret;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
	(void) rwlock;
	return 0;
}
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
	(void) rwlock;
	return 0;
}
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
	(void) rwlock;
	return 0;
}
