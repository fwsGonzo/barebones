#pragma once
#include <cstdint>

struct tls_table
{
  // thread self-pointer
  void* tls_data; // 0x0
  // per-cpu cpuid
  int cpuid;

  uintptr_t pad[3];
  uintptr_t guard; // _SENTINEL_VALUE_
};
extern tls_table tls;
