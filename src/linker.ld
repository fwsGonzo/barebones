/**
 * Inspired from the IncludeOS unikernel (https://github.com/hioa-cs/IncludeOS)
 *
 * This is the smallest possible linker script that supports
 * normal C and C++ operation, including global constructors,
 * exceptions and linker GC sections option.
**/
ENTRY(_start)

SECTIONS
{
  PROVIDE(_ELF_START_ = . + 0x100000);
  PROVIDE(_LOAD_START_ = _ELF_START_);
  . = _ELF_START_;

  .multiboot (_ELF_START_ ): {
      PROVIDE(_MULTIBOOT_START_ = .);
      KEEP(*(.multiboot))
   }

  .text ALIGN(0x10) :
  {
    _TEXT_START_ = .;
    *(.text*)
    *(.gnu.linkonce.t*)
    _TEXT_END_ = .;
  }

  .rodata :
  {
    _RODATA_START_ = .;
    *(.rodata*)
    *(.gnu.linkonce.r*)
    _RODATA_END_ = .;
  }

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  }
  /*.fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  }*/

  /* For stack unwinding (exception handling)  */
  .eh_frame_hdr ALIGN(0x8):
  {
    KEEP(*(.eh_frame_hdr*))
  }
  .eh_frame ALIGN(0x8):
  {
  	PROVIDE (__eh_frame_start = .);
    KEEP(*(.eh_frame))
    LONG (0);
  }
  .gcc_except_table :
  {
    *(.gcc_except_table)
  }

  .data :
  {
    _DATA_START_ = .;
    *(.data*)
    *(.gnu.linkonce.d*)
    _DATA_END_ = .;
  }

  PROVIDE(_LOAD_END_ = .);

  .bss :
  {
    _BSS_START_ = .;
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)
    _BSS_END_ = .;
  }

  . = ALIGN(0x10);
  _end = .;
}
