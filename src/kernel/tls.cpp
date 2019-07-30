#include <cstddef>
#include "tls.hpp"
#ifdef __x86_64__
static_assert(offsetof(tls_table, guard) == 0x28, "TLS is at 0x28 on amd64");
#elif __i386__
static_assert(offsetof(tls_table, guard) == 0x14, "TLS is at 0x18 on i386");
#endif

// we have to store this in .data otherwise .bss initialization
// will overwrite this in stdlib init
struct tls_table tls;
