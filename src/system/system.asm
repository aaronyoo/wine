; A collection of asm stubs used for system functions

global gdt_flush
gdt_flush:
   mov eax, [esp+4]  ; eax = gdt_ptr
   lgdt [eax]        ; load the GDT pointer

   mov ax, 0x10      ; 0x10 is kernel segment data descriptor offset
   mov ds, ax        
   mov es, ax
   mov fs, ax
   mov gs, ax
   mov ss, ax
   jmp 0x08:.flush   ; 0x08 is kernel segment code descriptor offset
                     ; far jump to .flush
.flush:
   ret


global idt_flush
idt_flush:
   mov eax, [esp+4]  ; eax = idt_ptr
   lidt[eax]         ; load the IDT ptr
   ret


global load_page_directory
load_page_dir:
   mov eax, [esp+4]
   mov cr3, eax
   ret

global get_curr_page_dir
get_curr_page_dir:
   mov eax, cr3
   ret