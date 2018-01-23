#include <cstddef>
#include <cstdint>
#include <cassert>
extern "C" void __serial_print1(const char*);

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
  __serial_print1("Hello from freestanding C++!\n");
  Test test(2);
  assert(test.value == 2);
}
