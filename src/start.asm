;; stack base address at EBDA border
;; NOTE: Multiboot can use 9d400 to 9ffff
%define  STACK_LOCATION     0x9D3F0

;; multiboot magic
%define  MB_MAGIC   0x1BADB002
%define  MB_FLAGS   0x3  ;; ALIGN + MEMINFO

extern _MULTIBOOT_START_
extern _LOAD_START_
extern _LOAD_END_
extern _end

ALIGN 4
section .multiboot
  dd  MB_MAGIC
  dd  MB_FLAGS
  dd  -(MB_MAGIC + MB_FLAGS)
  dd _MULTIBOOT_START_
  dd _LOAD_START_
  dd _LOAD_END_
  dd _end
  dd _start

section .text
global _start ;; make _start a global symbol
_start:
    cli
    ;; 32-bit stack ptr
    mov esp, STACK_LOCATION
    mov ebp, esp

    call enable_sse

    push eax
extern __init_serial1
    call __init_serial1
    pop eax
extern kernel_start
    push ebx
    push eax
    call kernel_start
    cli
    hlt

enable_sse:
    push eax
    mov eax, cr0
    and ax, 0xFFFB  ;clear coprocessor emulation CR0.EM
    or  ax, 0x2     ;set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9   ;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax
    pop eax
    ret
