#include <main.h>
#include <assert.h>
#include <kprint.h>
#include <x86intrin.h>
static bool test_sse();
static int  test_value = 0;

void kernel_main(const uint32_t eax, const uint32_t ebx)
{
	kprint("------------------\n");
	kprintf("* Multiboot EAX: 0x%x\n", eax);
	kprintf("* Multiboot EBX: 0x%x\n", ebx);
	// some helpful self-checks
	kprint("* SSE instructions ... ");
	kprint(test_sse() ? "work!\n" : "did NOT work!\n");
	kprint("* Global constructors ... ");
	kprint(test_value ? "work!\n" : "did NOT work!\n");

#ifdef UBSAN_ENABLED
	// when undefined sanitizer is enabled, we can manually trigger it like this
	uint64_t test = 0;
	char* misaligned = (char*) &test + 2;
	*(uint32_t*) misaligned = 0x12345678;
	kprintf("kernel_main is at %p\n", kernel_main);
	kprintf("misaligned is %p\n", (void*) test);
#endif

	kprint(
		"\n"
		"Hello OSdev world!\n"
		KERNEL_BINARY " (" KERNEL_DESC ")\n"
		"\n"
		"Press Ctrl+A -> X to close\n"
	);
}

__attribute__((constructor))
static void my_cpp_constructor() {
  test_value = 1;
}

bool test_sse()
{
  typedef union
  {
    __m128i i128;
    int32_t i32[4];
  } imm;
  volatile imm xmm1;
  xmm1.i128 = _mm_set_epi32(1, 2, 3, 4);
  return (
      xmm1.i32[0] == 4 && xmm1.i32[1] == 3 &&
      xmm1.i32[2] == 2 && xmm1.i32[3] == 1);
}
