;; stack base address at EBDA border
;; NOTE: Multiboot can use 9d400 to 9ffff
%define  STACK_LOCATION     0x9D400

;; multiboot magic
%define  MB_MAGIC   0x1BADB002
%define  MB_FLAGS   0x3  ;; ALIGN + MEMINFO

extern _MULTIBOOT_START_
extern _LOAD_START_
extern _LOAD_END_
extern _end
extern begin_enter_longmode
extern kernel_start

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

section .data
multiboot_data_magic:     dq 0
multiboot_data_address:   dq 0
global multiboot_data_magic
global multiboot_data_address

[BITS 32]
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

    ;; store multiboot params
	mov DWORD [multiboot_data_magic],    eax
	mov DWORD [multiboot_data_address],  ebx

    ;;ASM_PRINT(strings.phase1)
    call enable_cpu_feat

	;; zero .bss (used to be in the C portion, but easier to do here)
	extern _BSS_START_
	extern _BSS_END_
	mov edi, _BSS_START_
	mov ecx, _BSS_END_
	sub ecx, _BSS_START_
	mov eax, 0
	rep stosb

	;; Enable stack protector:
    ;; GS is located at 0x1000
    ;; Linux uses GS:0x14 to access stack protector value
    ;; Copy RDTSC.EAX to this location as preliminary value
    rdtsc
    mov DWORD [0x1014], eax

    ;; for 32-bit kernels just call kernel_start here
    call begin_enter_longmode
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
