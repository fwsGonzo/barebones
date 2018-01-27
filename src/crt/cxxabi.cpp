#include <new>
#include <cstddef>
#include <cstring>
#include <kprint.hpp>

//#define DEBUG_HEAP
#ifdef DEBUG_HEAP
#define HPRINT(fmt, ...) kprintf(fmt, __VA_ARGS__)
#else
#define HPRINT(fmt, ...) /* fmt */
#endif

void* operator new(size_t size)
{
  void* res = malloc(size);
  HPRINT("operator new: size %u => %p\n", size, res);
  return res;
}
void* operator new[](size_t size)
{
  void* res = malloc(size);
  HPRINT("operator new[]: size %u => %p\n", size, res);
  return res;
}

void operator delete(void* ptr)
{
  HPRINT("operator delete: %p\n", ptr);
  free(ptr);
}
void operator delete[](void* ptr)
{
  HPRINT("operator delete[]: %p\n", ptr);
  free(ptr);
}

extern "C" void __cxa_pure_virtual()
{
  kprintf("Unimplemented pure virtual function called\n");
}

/// EASTL glue ///

void* operator new[] (
        size_t size, /* size */
        const char*, /* pName */
        int,         /* flags */
        unsigned,    /* debugFlags */
        const char*, /* file */
        int)         /* line */
{
  void* res = malloc(size);
  HPRINT("eastl new: size: %u = %p\n", size, res);
  return res;
}
void* operator new[] (
        size_t size,  /* size */
        size_t align, /* alignment */
        size_t,       /* alignmentOffset */
        const char*,  /* pName */
        int,          /* flags */
        unsigned,     /* debugFlags */
        const char*,  /* file */
        int)          /* line */
{
  void* res = malloc(size);
  HPRINT("eastl aligned new: size %u align %u => %p\n",
          size, align, res);
  (void) align;
  return res;
}
