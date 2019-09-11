ENTRY(start)

KERNEL_VIRT_START = 0xC0100000;

SECTIONS {
    . = KERNEL_VIRT_START;
	.text ALIGN (0x1000) : AT(ADDR(.text)-0xC0000000)
	{
        *(.multiboot)
		*(.text)
	}
    
    .rodata ALIGN (0x1000) : AT(ADDR(.rodata)-0xC0000000)
    {
    	*(.rodata)
    }

    .data ALIGN (0x1000) : AT(ADDR(.data)-0xC0000000)
    {
    	*(.data)
    }

    .bss ALIGN (0x1000) : AT(ADDR(.bss)-0xC0000000)
    {
    	*(.bss)
    }
}