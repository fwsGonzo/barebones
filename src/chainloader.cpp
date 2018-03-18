#include <kprint.hpp>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <multiboot.h>
#include <elf.h>

//#define DEBUG
#ifdef DEBUG
#define PRINT(X,...)  kprintf(X,##__VA_ARGS__);
#else
#define PRINT(X,...)
#endif

extern "C" void hotswap(const char* base, int len, char* dest, void* start,
                        uintptr_t magic, uintptr_t bootinfo);

struct mods_t {
  multiboot_module_t* mod;
  uint32_t          count;
};

static multiboot_info_t* bootinfo(uint32_t addr)
{
  return (multiboot_info_t*) addr;
}
static mods_t modules(const multiboot_info_t* info)
{
  if (info and info->flags & MULTIBOOT_INFO_MODS && info->mods_count > 0)
  {
    return mods_t {(multiboot_module_t*) info->mods_addr, info->mods_count};
  }
  return mods_t{nullptr, 0};
}

void promote_mod_to_kernel(multiboot_info_t* info)
{
  assert(info->mods_count > 0);
  auto* mod = (multiboot_module_t*) info->mods_addr;

  // move commandline to a relatively safe area
  const uint32_t RELATIVELY_SAFE_AREA = 0x8000;
  strcpy((char*) RELATIVELY_SAFE_AREA, (const char*) mod->cmdline);
  info->cmdline = RELATIVELY_SAFE_AREA;

  // Subtract one module
  info->mods_count--;

  if (info->mods_count)
    info->mods_addr = (uint32_t) ((multiboot_module_t*)info->mods_addr + 1);
}

extern "C"
void main(uint32_t mb_magic, uint32_t mb_addr)
{
  mods_t mods = modules(bootinfo(mb_addr));
  PRINT("\n%u-bit chainloader found %u modules\n",
        sizeof(void*) * 8, mods.count);

  if (mods.count < 1) {
    kprintf("\nchainloader: No modules passed to multiboot!\n");
    return;
  }

  multiboot_module_t binary = mods.mod[0];

  // treat module as 64-bit ELF
  auto* elf = (Elf64_Ehdr*) binary.mod_start;
  auto* phdr = (Elf64_Phdr*) (binary.mod_start + elf->e_phoff);
  // program header should be valid & loadable
  assert(phdr->p_paddr > 0xFFF && phdr->p_type == PT_LOAD);
  // program headers
  const uint32_t program_headers = elf->e_phnum;

  // update multiboot info for new kernel
  promote_mod_to_kernel(bootinfo(mb_addr));

  PRINT("Preparing for jump to %s. Multiboot magic: 0x%x, addr 0x%x\n",
        (char*) binary.cmdline, mb_magic, mb_addr);

  // measure & allocate kernel buffer
  int32_t len = 0;
  for (const auto* hdr = phdr; hdr < phdr + program_headers; hdr++)
  {
    uint64_t end = hdr->p_paddr + hdr->p_memsz;
    if (len < end) len = end;
  }
  len -= phdr->p_offset;
  PRINT("Length in memory: %d (%d kB)\n", len, len / 1024);

  char* kernel = new char[len];

  // build kernel
  for (const auto* hdr = phdr; hdr < phdr + program_headers; hdr++)
  {
    const char* src = (char*) elf + hdr->p_offset;
    int offset = hdr->p_paddr - phdr->p_paddr;
    memcpy(&kernel[offset], src, hdr->p_filesz);
    PRINT("Segment src: %p offset: %d -> dst: %p\n", src, offset, &kernel[offset]);
  }

  // Move hotswap function away from binary
  void* hotswap_addr = (void*)0x2000;
  extern char __hotswap_end;
  memcpy(hotswap_addr,(void*)&hotswap, &__hotswap_end - (char*)&hotswap );

  // Prepare to load ELF segment
  const char* base  = kernel;
  char* dest  = (char*) phdr->p_paddr;
  void* start = (void*) elf->e_entry;

  asm("cli");
  PRINT("Jumping to %p located in %p:%d\n", start, dest, len);

  ((decltype(&hotswap))hotswap_addr)(base, len, dest, start, mb_magic, mb_addr);
  __builtin_unreachable();
}
