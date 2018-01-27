#include <EASTL/vector.h>
#include <cassert>
#include <kprint.hpp>

struct Test
{
  Test(int x) {
    vec.push_back(x);
  }

  void add(int v) {
    vec.push_back(v);
  }
  int back() const {
    return vec.back();
  }

  eastl::vector<int> vec;
};

static void test_eastl()
{
  kprintf("Checking EASTL support\n");
  Test test(1);
  assert(test.back() == 1);
  test.add(2);
  assert(test.back() == 2);
  test.add(4);
  assert(test.back() == 4);
  kprintf("EASTL vector works!\n");
}

extern "C"
void test_cpp()
{
  test_eastl();

  kprintf("Hello from freestanding C++!\n");
}
