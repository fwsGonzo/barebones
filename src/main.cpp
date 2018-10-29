#include "main.hpp"
#include <assert.h>
#include <kprint.hpp>
#include <x86intrin.h>

void kernel_main(const uint32_t eax, const uint32_t ebx)
{
  kprint("------------------\n");
  kprint("Hello OSdev world!\n");

  kprintf("Multiboot EAX: 0x%x\n", eax);
  kprintf("Multiboot EBX: 0x%x\n", ebx);

  void test_sse();
  test_sse();

  kprint("Press Ctrl+A -> X to close\n");
}

void test_sse()
{
  typedef union
  {
    __m128i i128;
    int32_t i32[4];
  } imm;
  volatile imm xmm1;
  xmm1.i128 = _mm_set_epi32(1, 2, 3, 4);
  assert(xmm1.i32[0] == 4 && xmm1.i32[1] == 3);
  assert(xmm1.i32[2] == 2 && xmm1.i32[3] == 1);
  kprintf("* SSE works!\n");
}
