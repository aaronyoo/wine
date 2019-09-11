#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <stdint.h>

#define PAGE_SIZE 4096
#define BIT_SIZE_OF_UINT32 32
#define PAGE_FAULT_INT_NUM 14

//====================
// Physical Addresses
//====================

const uint32_t MEM_PHYS_KERNEL_START = 0x00100000;
const uint32_t MEM_PHYS_KERNEL_END = 0x00300000;

// ~250 Mb for the page frame area, this can be expanded later if needed
const uint32_t MEM_PHYS_PAGE_FRAME_AREA_START = 0x00300000;
const uint32_t MEM_PHYS_PAGE_FRAME_AREA_END = 0x0F000000;

//====================
// Virtual Addresses
//====================

const uint32_t MEM_VIRT_KERNEL_START = 0xC0100000;
const uint32_t MEM_VIRT_KERNEL_END = 0xC0300000;

// ~6 Mb for premapped page tables, this can be expanded later if needed
const uint32_t MEM_VIRT_PAGE_TABLE_AREA_START = 0xC0300000;
const uint32_t MEM_VIRT_PAGE_TABLE_AREA_END = 0xC0900000;

// ~140 Mb for kernel heap, this may actually be too large but better be safe
const uint32_t MEM_VIRT_KHEAP_START = 0xC0900000;
const uint32_t MEM_VIRT_KHEAP_END = 0xC9000000;

//====================
// Virtual Addresses
//====================

typedef uint32_t phys_addr_t;
typedef uint32_t* virt_addr_t;
typedef uint32_t* page_table_t;
typedef uint32_t* page_dir_t;

#endif // MEMORY_HPP