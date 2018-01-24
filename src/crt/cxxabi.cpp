#include <new>
#include <kprint.hpp>
#include <stddef.h>

void* operator new(size_t size)
{
  return malloc(size);
}
void* operator new[](size_t size)
{
  return malloc(size);
}

void operator delete(void* ptr)
{
  free(ptr);
}
void operator delete[](void* ptr)
{
  free(ptr);
}

extern "C" void __cxa_pure_virtual()
{
    kprintf("Unimplemented pure virtual function called\n");
}
