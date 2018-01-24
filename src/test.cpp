#include <cassert>
#include <kprint.hpp>

struct Test
{
  Test(int x) {
    this->value = x;
  }
  int value;
};

extern "C"
void test_cpp()
{
  auto* t = new Test(2);
  assert(t->value == 2);
  delete t;
  kprintf("Hello from freestanding C++!\n");
}
