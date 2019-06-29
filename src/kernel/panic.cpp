#include <kprint.h>
#include <cstdio>
// less risky when the stack is blown out
static char buffer[4096];

#define frp(N, ra)                                 \
  (__builtin_frame_address(N) != nullptr) &&       \
  (ra = __builtin_return_address(N)) != nullptr && ra != (void*)-1

static void print_trace(const int N, const void* ra)
{
  snprintf(buffer, sizeof(buffer),
          "[%d] %p\n",
          N, ra);
  kprint(buffer);
}

extern "C"
void print_backtrace()
{
  kprintf("\nBacktrace:\n");
  void* ra;
  if (frp(0, ra)) {
    print_trace(0, ra);
    if (frp(1, ra)) {
      print_trace(1, ra);
      if (frp(2, ra)) {
        print_trace(2, ra);
      }
    }
  }
}

extern "C"
__attribute__((noreturn))
void panic(const char* reason)
{
	kprintf("\n\n!!! PANIC !!!\n%s\n", reason);

	print_backtrace();

	// the end
	kprintf("\nKernel halting...\n");
	while (1) asm("cli; hlt");
	__builtin_unreachable();
}

extern "C"
void abort()
{
	panic("Abort called");
}

extern "C"
void abort_message(const char* fmt, ...)
{
	va_list arg;
	va_start (arg, fmt);
	int bytes = tfp_vsnprintf(buffer, sizeof(buffer), fmt, arg);
	(void) bytes;
	va_end (arg);
	panic(buffer);
}
