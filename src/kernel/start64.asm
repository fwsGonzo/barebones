[BITS 32]
global begin_enter_longmode:function
extern __serial_print1
extern kernel_start

%define STACK_LOCATION          0x9D000
%define P4_TAB                  0x1000 ;; one page
%define P3_TAB                  0x2000 ;; one page
%define P2_TAB                  0x3000 ;; many pages

%define NUM_P3_ENTRIES     4
%define NUM_P2_ENTRIES     (NUM_P3_ENTRIES * 512)

%define IA32_EFER               0xC0000080
%define IA32_STAR               0xC0000081
%define IA32_LSTAR              0xc0000082
%define IA32_FMASK              0xc0000084
%define IA32_FS_BASE            0xc0000100
%define IA32_GS_BASE            0xc0000101
%define IA32_KERNEL_GS_BASE     0xc0000102

;; CR0 paging enable bit
%define PAGING_ENABLE 0x80000000
;; CR0 Supervisor write-protect enable
%define SUPER_WP_ENABLE 0x10000

;; EFER Longmode bit
%define LONGMODE_ENABLE 0x100
;; EFER Execute Disable bit
%define NX_ENABLE 0x800
;; EFER Syscall enable bit
%define SYSCALL_ENABLE 0x1

%macro ASM_PRINT 1
  push %1
  call __serial_print1
  add esp, 4
%endmacro

extern multiboot_data_location
extern multiboot_data_address

SECTION .text
begin_enter_longmode:
    ;; disable old paging
    mov eax, cr0
    and eax, 0x7fffffff  ;; clear PG (bit 31)
    mov cr0, eax

    ;; address for Page Map Level 4
    mov edi, P4_TAB
    mov cr3, edi
    ;; clear out P4 and P3
    mov ecx, 0x2000 / 0x4
    xor eax, eax       ; Nullify the A-register.
    rep stosd

    ;; create page map entry
    mov edi, P4_TAB
    mov DWORD [edi], P3_TAB | 0x3 ;; present+write

    ;; create 1GB mappings
    mov ecx, NUM_P3_ENTRIES
    mov edi, P3_TAB
    mov eax, P2_TAB | 0x3 ;; present + write
    mov ebx, 0x0

.p3_loop:
    mov DWORD [edi],   eax   ;; Low word
    mov DWORD [edi+4], ebx   ;; High word
    add eax, 1 << 12         ;; page increments
    adc ebx, 0               ;; increment high word when CF set
    add edi, 8
    loop .p3_loop

    ;; create 2MB mappings
    mov ecx, NUM_P2_ENTRIES
    mov edi, P2_TAB
    mov eax, 0x0 | 0x3 | 1 << 7 ;; present + write + huge
    mov ebx, 0x0

.p2_loop:
    mov DWORD [edi],   eax   ;; Low word
    mov DWORD [edi+4], ebx   ;; High word
    add eax, 1 << 21         ;; 2MB increments
    adc ebx, 0               ;; increment high word when CF set
    add edi, 8
    loop .p2_loop

    ;; enable PAE
    mov eax, cr4
    or  eax, 1 << 5
    mov cr4, eax

    ;; enable long mode
    mov ecx, IA32_EFER
    rdmsr
    or  eax, (LONGMODE_ENABLE | NX_ENABLE | SYSCALL_ENABLE)
    wrmsr

    ;; enable paging
    mov eax, cr0                 ; Set the A-register to control register 0.
    or  eax, (PAGING_ENABLE | SUPER_WP_ENABLE)
    mov cr0, eax                 ; Set control register 0 to the A-register.

    ;; load 64-bit GDT
    lgdt [__gdt64_base_pointer]
    jmp  GDT64.Code:long_mode


[BITS 64]
long_mode:
    cli

    ;; segment regs
    mov cx, GDT64.Data
    mov ds, cx
    mov es, cx
    mov fs, cx
    mov gs, cx
    mov ss, cx

    ;; set up new stack for 64-bit
    push rsp
    mov  rsp, STACK_LOCATION
    push 0
    push 0
    mov  rbp, rsp

    mov ecx, IA32_STAR
    mov edx, 0x8
    mov eax, 0x0
    wrmsr

    ;; Enable stack protector:
    ;; On amd64 FS should point to tls-table for cpu0
    ;; Linux uses FS:0x28 to access stack protector value
    extern tls
    mov ecx, IA32_FS_BASE
    mov edx, 0x0
    mov eax, tls
    wrmsr
    ;; Set "random" stack protector value
    extern __SSP__
    rdtsc
    mov rcx, __SSP__
    xor rax, rcx
    ;; Install in TLS table
    mov QWORD [tls+0x28], rax

    ;; geronimo!
    mov  edi, DWORD[multiboot_data_location]
    mov  esi, DWORD[multiboot_data_address]
    call kernel_start
    ;; warning that we returned from kernel_start
    mov rdi, strings.panic
    call __serial_print1
    cli
    hlt

SECTION .data
strings:
  .panic: db `Returned from kernel_start! Halting...\n`,0x0
GDT64:
  .Null: equ $ - GDT64         ; The null descriptor.
    dq 0
  .Code: equ $ - GDT64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 00100000b                 ; Granularity.
    db 0                         ; Base (high).
  .Data: equ $ - GDT64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
  .Task: equ $ - GDT64         ; TSS descriptor.
    dq 0
    dq 0

    dw 0x0 ;; alignment padding
__gdt64_base_pointer:
    dw $ - GDT64 - 1             ; Limit.
    dq GDT64                     ; Base.
