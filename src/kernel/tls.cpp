#include <cstddef>
extern "C" {
#include "tls.h"
}
static_assert(offsetof(tls_table, guard) == 0x28, "TLS is at 0x28 on amd64");

// we have to store this in .data otherwise .bss initialization
// will overwrite this in stdlib init
struct tls_table tls __attribute__((section(".data")));
