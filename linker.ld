/**
 * This file is a part of the IncludeOS unikernel - www.includeos.org
 *
 * Copyright 2015 Oslo and Akershus University College of Applied Sciences
 * and Alfred Bratterud
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http: *www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/
ENTRY(_start)

SECTIONS
{
  PROVIDE ( _ELF_START_ = . + 0x100000);
  PROVIDE ( _LOAD_START_ = _ELF_START_);
  . = _ELF_START_;

  .multiboot (_ELF_START_ ): {
      PROVIDE(_MULTIBOOT_START_ = .);
      *(.multiboot)
   }

  PROVIDE( _TEXT_START_ = . );
  .text ALIGN(0x10) :
  {
    *(.text*)
    *(.gnu.linkonce.t*)
  }
  PROVIDE( _TEXT_END_ = . );

  .rodata :
  {
    _RODATA_START_ = .;
    *(.rodata*)
    *(.gnu.linkonce.r*)
    _RODATA_END_ = .;
  }

  .data :
  {
    _DATA_START_ = .;
    *(.data*)
    *(.gnu.linkonce.d*)
    _DATA_END_ = .;
  }

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  }

  PROVIDE (_LOAD_END_ = .);

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
