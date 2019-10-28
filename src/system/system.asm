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


global load_page_dir
load_page_dir:
   mov eax, [esp+4]
   mov cr3, eax
   ret

global get_curr_page_dir
get_curr_page_dir:
   mov eax, cr3
   ret

global get_cr2
get_cr2:
   mov eax, cr2
   ret

global get_cr3
get_cr3:
   mov eax, cr3
   ret

extern current_task
global switch_to_task_stub
; Switch tasks:
; Old task is in current and new task is at [esp+4] (first arg)
switch_to_task_stub:
   ; save registers for the old task
   push ebx
   push esi
   push edi
   push ebp

   mov edi, current_task  ; Move old task into edi
   mov [edi+12], esp  ; kernel_stack_top = esp
   
   mov esi, [esp+4]  ; load the new task
   mov esp, [esi+12] ; reload the kernel_stack_top
   mov eax, [esi+16] ; eax = new task page directory

   mov ecx, cr3
   cmp eax, ecx      ; compare old and new page directories
   je .doneVAS
   mov cr3, eax      ; only change page directory if it is new

.doneVAS

   pop ebp
   pop edi
   pop esi
   pop ebx

   ret   ; Load next task's EIP from its kernel stack
         ; this means that the kernel stack must be bootstraped with
         ; and instruction pointer.


