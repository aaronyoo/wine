bits 32

;------------------
; Multiboot Header
;------------------
MAGIC    equ  0x1BADB002
MODALIGN equ  1 << 0
MEMMAP   equ  1 << 1
FLAGS    equ  MODALIGN | MEMMAP
CHECKSUM equ  -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

;---------------
; Kernel Stack
;---------------
section .bss
align 4096
boot_page_directory:
    resb 4096
boot_page_table1:
    resb 4096
boot_page_table2:
    resb 4096
kernel_stack_bottom: equ $          ; Create a 16KB stack for the kernel
	resb 16384 ; 16 KB
kernel_stack_top:


KERNEL_VIRT_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRT_BASE >> 22)   ; Index of PTE in page directory
PAGE_SIZE equ 4096

;--------------------
; Kernel Entry Point
;--------------------
section .text
global start
start:
    ; Create a page table that identity maps to the first ~4 Mib worth of memory.
    ; May need to add more initial page tables of the kernel ever exceeds ~4 Mib.
    mov edi, (boot_page_table1 - KERNEL_VIRT_BASE)
    mov esi, 0
    mov ecx, 1024   ; Map 1024 entries (a full page table) ~4 Mib worth of memory

L1:
    cmp ecx, 0
    jle end

    mov edx, esi
    or edx, 3
    mov [edi], edx

    add esi, 4096   ; Go to the next page address
    add edi, 4      ; Go to the next page table entry
    sub ecx, 1
    jmp L1

end:

    ; Now we need to copy the contents of boot_page_table1 into boot_page_table2
    ; so that we can have two identical mappings (one in the lower half and one in
    ; the upper half). It is very important to make a copy and not simply use a
    ; pointer here. If we use a pointer, we run into a situation later in
    ; which unmapping the lower kernel results in umapping the upper kernel as
    ; well.
    mov esi, boot_page_table1 - KERNEL_VIRT_BASE
    mov edi, boot_page_table2 - KERNEL_VIRT_BASE
    mov ecx, 4096
L2:
    mov dl, [esi]
    mov [edi], dl
    inc esi
    inc edi
    loop L2

    ; Put the page tables into the boot_page_directory. Even though they have
    ; the same contents we are using separate page tables for the lower mapped and
    ; upper mapped kernel.
    mov DWORD[boot_page_directory - KERNEL_VIRT_BASE + 0], boot_page_table1 - KERNEL_VIRT_BASE + 3
    mov DWORD[boot_page_directory - KERNEL_VIRT_BASE + 768 * 4], boot_page_table2 - KERNEL_VIRT_BASE + 3

    mov ecx, (boot_page_directory - KERNEL_VIRT_BASE)
    mov cr3, ecx    ; Initialize the page directory base

    mov ecx, cr0
    or ecx, 0x80000000    ; Set PG bit in CR0 to enable paging
    mov cr0, ecx

    ; Long jump to the higher half
    lea ecx, [higher_half]
    jmp ecx

higher_half:
    ; Now we are in the higher half. Reload CR3 using higher half page directory
    mov ecx, (boot_page_directory - KERNEL_VIRT_BASE)
    mov cr3, ecx

    mov esp, kernel_stack_top       ; Set up the stack
    push eax;                       ; Push multiboot magic
    push ebx;                       ; Push multiboot header

    extern main
    call main               ; Jump to the kernel (never to return)

    cli                             ; We should not return here, but if we do
    hlt                             ; clear all interrupts and halt