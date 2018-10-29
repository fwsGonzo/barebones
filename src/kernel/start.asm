extern __serial_print1
extern __init_stdlib
extern kernel_start

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

%macro ASM_PRINT 1
  pusha
  push %1
  call __serial_print1
  add esp, 4
  popa
%endmacro

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

    ;; load simple GDT
    lgdt [gdtr]

    ;; activate new GDT
    jmp 0x8:rock_bottom ;; code seg
rock_bottom:
    mov cx, 0x10 ;; data seg
    mov ss, cx
    mov ds, cx
    mov es, cx
    mov fs, cx
    mov cx, 0x18 ;; GS seg
    mov gs, cx

    ;; 32-bit stack ptr
    mov esp, STACK_LOCATION
    mov ebp, esp

    ;; align stack to 16 bytes
    sub esp, 8
    ;; store multiboot params
    push ebx
    push eax

    ;;ASM_PRINT(strings.phase1)
    call enable_cpu_feat

    ;; Enable stack protector:
    ;; GS is located at 0x1000
    ;; Linux uses GS:0x14 to access stack protector value
    ;; Copy RDTSC.EAX to this location as preliminary value
    rdtsc
    mov DWORD [0x1014], eax
    ;;ASM_PRINT(strings.phase2)

    ;; eax, ebx still on stack
    call kernel_start
    add esp, 8
    ;; warning that we returned from kernel_start
    push strings.panic
    call __serial_print1
    ;; stop
    cli
    hlt

enable_cpu_feat:
    ;; enable SSE (pretty much always exists)
    mov eax, cr0
    and ax, 0xFFFB  ;clear coprocessor emulation CR0.EM
    or  ax, 0x2     ;set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9   ;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    or ax, 0x20     ;enable native FPU exception handling
    mov cr4, eax
    ;; read out CPU features
    mov eax, 1
    xor ecx, ecx
    cpuid
    mov edx, ecx
    ;; check for XSAVE support (bit 26)
    and ecx, 0x04000000
    jz xsave_not_supported
    ;; enable XSAVE
    mov eax, cr4
    or  eax, 0x40000
    mov cr4, eax
    ;; check for AVX support (bit 28)
    and edx, 0x10000000
    jz xsave_not_supported
    ;; enable AVX
    xor ecx, ecx
    xgetbv
    or eax, 0x7
    xsetbv
xsave_not_supported:
    ret

section .data
strings:
  .panic: db `Returned from kernel_start! Halting...\n`,0x0
  .phase1: db `start.asm PHASE 1\n`,0x0
  .phase2: db `start.asm PHASE 2\n`,0x0

ALIGN 32
gdtr:
  dw gdt32_end - gdt32 - 1
  dd gdt32
ALIGN 32
gdt32:
  ;; Entry 0x0: Null descriptor
  dq 0x0
  ;; Entry 0x8: Code segment
  dw 0xffff          ;Limit
  dw 0x0000          ;Base 15:00
  db 0x00            ;Base 23:16
  dw 0xcf9a          ;Flags / Limit / Type [F,L,F,Type]
  db 0x00            ;Base 32:24
  ;; Entry 0x10: Data segment
  dw 0xffff          ;Limit
  dw 0x0000          ;Base 15:00
  db 0x00            ;Base 23:16
  dw 0xcf92          ;Flags / Limit / Type [F,L,F,Type]
  db 0x00            ;Base 32:24
  ;; Entry 0x18: GS Data segment
  dw 0x0100          ;Limit
  dw 0x1000          ;Base 15:00
  db 0x00            ;Base 23:16
  dw 0x4092          ;Flags / Limit / Type [F,L,F,Type]
  db 0x00            ;Base 32:24
gdt32_end:
