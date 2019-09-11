#include "idt.hpp"
#include "lib/cstring.hpp"

#define IDT_ENTRIES 256
idt_entry_t idt_tbl[IDT_ENTRIES];
idt_ptr_t idt_ptr;

extern "C" void idt_flush(uint32_t);
extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();

void idt::set_gate(uint8_t n, uint32_t base, uint16_t sel, uint8_t flags) {
	idt_tbl[n].base_lo = base & 0xFFFF;
	idt_tbl[n].base_hi = (base >> 16) & 0xFFFF;
	idt_tbl[n].sel = sel;
	idt_tbl[n].res = 0;

	// TODO: maybe need to | 0x60 later when I create a userspace
	idt_tbl[n].flags = flags;
}

void idt::init() {
	// Populate IDT ptr.
	idt_ptr.limit = sizeof(idt_entry_t) * IDT_ENTRIES;
	idt_ptr.base = (uint32_t)idt_tbl; 

	// Clean up IDT table.
	memset(idt_tbl, 0, sizeof(idt_entry_t) * IDT_ENTRIES);

	// Set gates to be ISRs. PIC should be initialized and
	// remapping should be done by now so 32-27 are hardware
	// interrupts from the PIC. 0-31 are reserved for software
	// interrupts. See Intel manual for details.
	idt::set_gate(0, (uint32_t) isr0, 0x08, 0x8E);
	idt::set_gate(1, (uint32_t) isr1, 0x08, 0x8E);
	idt::set_gate(2, (uint32_t) isr2, 0x08, 0x8E);
	idt::set_gate(3, (uint32_t) isr3, 0x08, 0x8E);
	idt::set_gate(4, (uint32_t) isr4, 0x08, 0x8E);
	idt::set_gate(5, (uint32_t) isr5, 0x08, 0x8E);
	idt::set_gate(6, (uint32_t) isr6, 0x08, 0x8E);
	idt::set_gate(7, (uint32_t) isr7, 0x08, 0x8E);
	idt::set_gate(8, (uint32_t) isr8, 0x08, 0x8E);
	idt::set_gate(9, (uint32_t) isr9, 0x08, 0x8E);
	idt::set_gate(10, (uint32_t) isr10, 0x08, 0x8E);
	idt::set_gate(11, (uint32_t) isr11, 0x08, 0x8E);
	idt::set_gate(12, (uint32_t) isr12, 0x08, 0x8E);
	idt::set_gate(13, (uint32_t) isr13, 0x08, 0x8E);
	idt::set_gate(14, (uint32_t) isr14, 0x08, 0x8E);
	idt::set_gate(15, (uint32_t) isr15, 0x08, 0x8E);
	idt::set_gate(16, (uint32_t) isr16, 0x08, 0x8E);
	idt::set_gate(17, (uint32_t) isr17, 0x08, 0x8E);
	idt::set_gate(18, (uint32_t) isr18, 0x08, 0x8E);
	idt::set_gate(19, (uint32_t) isr19, 0x08, 0x8E);
	idt::set_gate(20, (uint32_t) isr20, 0x08, 0x8E);
	idt::set_gate(21, (uint32_t) isr21, 0x08, 0x8E);
	idt::set_gate(22, (uint32_t) isr22, 0x08, 0x8E);
	idt::set_gate(23, (uint32_t) isr23, 0x08, 0x8E);
	idt::set_gate(24, (uint32_t) isr24, 0x08, 0x8E);
	idt::set_gate(25, (uint32_t) isr25, 0x08, 0x8E);
	idt::set_gate(26, (uint32_t) isr26, 0x08, 0x8E);
	idt::set_gate(27, (uint32_t) isr27, 0x08, 0x8E);
	idt::set_gate(28, (uint32_t) isr28, 0x08, 0x8E);
	idt::set_gate(29, (uint32_t) isr29, 0x08, 0x8E);
	idt::set_gate(30, (uint32_t) isr30, 0x08, 0x8E);
	idt::set_gate(31, (uint32_t) isr31, 0x08, 0x8E);
	idt::set_gate(32, (uint32_t) irq0, 0x08, 0x8E);
	idt::set_gate(33, (uint32_t) irq1, 0x08, 0x8E);
	idt::set_gate(34, (uint32_t) irq2, 0x08, 0x8E);
	idt::set_gate(35, (uint32_t) irq3, 0x08, 0x8E);
	idt::set_gate(36, (uint32_t) irq4, 0x08, 0x8E);
	idt::set_gate(37, (uint32_t) irq5, 0x08, 0x8E);
	idt::set_gate(38, (uint32_t) irq6, 0x08, 0x8E);
	idt::set_gate(39, (uint32_t) irq7, 0x08, 0x8E);
	idt::set_gate(40, (uint32_t) irq8, 0x08, 0x8E);
	idt::set_gate(41, (uint32_t) irq9, 0x08, 0x8E);
	idt::set_gate(42, (uint32_t) irq10, 0x08, 0x8E);
	idt::set_gate(43, (uint32_t) irq11, 0x08, 0x8E);
	idt::set_gate(44, (uint32_t) irq12, 0x08, 0x8E);
	idt::set_gate(45, (uint32_t) irq13, 0x08, 0x8E);
	idt::set_gate(46, (uint32_t) irq14, 0x08, 0x8E);
	idt::set_gate(47, (uint32_t) irq15, 0x08, 0x8E);

	idt_flush((uint32_t) &idt_ptr);
}