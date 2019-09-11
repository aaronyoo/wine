#include "gdt.hpp"

#define GDT_NUM_ENTRIES 5
gdt_ptr_t gdt_ptr;
gdt_entry_t gdt_entries[GDT_NUM_ENTRIES];

#define SEG_CODE_PL0 0x9A
#define SEG_DATA_PL0 0x92
#define SEG_CODE_PL3 0xFA
#define SEG_DATA_PL3 0xF2

extern "C" void gdt_flush(uint32_t);

void gdt::set_gate(int32_t num, uint32_t base, uint32_t limit, 
                         uint32_t access, uint32_t granularity) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= granularity & 0xF0;
    gdt_entries[num].access      = access;
}

void gdt::init() {
    // Populate the GDT pointer.
    gdt_ptr.size = (sizeof(gdt_entry_t) * GDT_NUM_ENTRIES) - 1;
    gdt_ptr.address = (uint32_t) &gdt_entries;

    // Set up GDT descriptors.
    gdt::set_gate(0, 0, 0, 0, 0);  // null descriptor
    gdt::set_gate(1, 0, 0xFFFFFFFF, SEG_CODE_PL0, 0xCF);  // kernel mode code
    gdt::set_gate(2, 0, 0xFFFFFFFF, SEG_DATA_PL0, 0xCF);  // kernel mode data
    gdt::set_gate(3, 0, 0xFFFFFFFF, SEG_CODE_PL3, 0xCF);  // user mode code
    gdt::set_gate(4, 0, 0xFFFFFFFF, SEG_DATA_PL3, 0xCF);  // user mode data segment

    // TODO: I think I need to add a TSS here for context switching

    // Start using the new GDT.
    gdt_flush((uint32_t) &gdt_ptr);
}