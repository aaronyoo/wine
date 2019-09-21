#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <stdint.h>

namespace memory {

const uint32_t PAGE_SIZE = 4096;
const uint32_t UINT8_BITS = 8;

//=============================================================================
// Memory Constants
//=============================================================================

const uint32_t MEM_PHYS_KERNEL_START = 0x00100000;
const uint32_t MEM_PHYS_KERNEL_END = 0x00400000;

const uint32_t MEM_PHYS_KERNEL_BASE = 0xC0000000;

// ~250 Mb for the page frame and page table area, this can be expanded later if
// needed
const uint32_t MEM_PHYS_PAGE_TABLE_RESERVED_AREA_START = 0x00400000;
const uint32_t MEM_PHYS_PAGE_TABLE_RESERVED_AREA_END = 0x00700000;
const uint32_t MEM_PHYS_PAGE_FRAME_AREA_START = 0x00700000;
const uint32_t MEM_PHYS_PAGE_FRAME_AREA_END = 0x0F000000;

const uint32_t MEM_VIRT_KERNEL_START = 0xC0100000;
const uint32_t MEM_VIRT_KERNEL_END = 0xC0400000;

// ~3 Mb for premapped page tables, this can be expanded later if needed
const uint32_t MEM_VIRT_PAGE_TABLE_RESERVED_AREA_START = 0xC0400000;
const uint32_t MEM_VIRT_PAGE_TABLE_RESERVED_AREA_END = 0xC0700000;

// ~140 Mb for kernel heap, this may actually be too large but better be safe
const uint32_t MEM_VIRT_KHEAP_START = 0xC0700000;
const uint32_t MEM_VIRT_KHEAP_END = 0xC9000000;

//=============================================================================
// Address Types
//=============================================================================

// typedef uint32_t phys_addr_t;
// typedef uint32_t virt_addr_t;
// typedef uint32_t* page_table_t;
// typedef uint32_t* page_dir_t;

void init();

} // namespace memory

#endif // MEMORY_HPP