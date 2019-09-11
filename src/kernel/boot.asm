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
align 32
kernel_stack_bottom: equ $          ; Create a 16KB stack for the kernel
	resb 16384 ; 16 KB
kernel_stack_top:

;----------------------
; Initial Page Tables
;----------------------
KERNEL_VIRT_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRT_BASE >> 22)   ; Index of PTE in page directory

section .data
align 4096
boot_page_directory:
    dd 0x00000083   ; page table for the initial kernel mapping
    times (KERNEL_PAGE_NUMBER - 1) dd 0  ; page tables after the initial mapping
    dd 0x00000083   ; page table for the higher half kernel
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0  ; page tables after the higher half kernel
 
;--------------------
; Kernel Entry Point
;--------------------
section .text
global start
start:
    mov ecx, (boot_page_directory - KERNEL_VIRT_BASE)
    mov cr3, ecx    ; Initialize the page directory base

    ; TODO: got to map the pages here
    mov ecx, cr4
    or ecx, 0x00000010    ; Enable 4MB paging by setting PSE bit in CR4
    mov cr4, ecx

    mov ecx, cr0
    or ecx, 0x80000000    ; Set PG bit in CR0 to enable paging
    mov cr0, ecx

    ; Long jump to the higher half
    lea ecx, [higher_half]
    jmp ecx

higher_half:
    mov esp, kernel_stack_top       ; Set up the stack
    push eax;                       ; Push multiboot magic
    push ebx;                       ; Push multiboot header

    extern main
    call main               ; Jump to the kernel (never to return)

    cli                             ; We should not return here, but if we do
    hlt                             ; clear all interrupts and halt