#include "memory.hpp"
#include "kernel/logger/logger.hpp"
#include "system/isr.hpp"
#include "lib/cstring.hpp"

namespace memory {

// A page table to bootstrap the mapping of the page table reserved pages
uint32_t ptr_page_table[1024] __attribute__((aligned(PAGE_SIZE)));

extern "C" void load_page_dir(uint32_t);
extern "C" void enable_paging();
extern "C" uint32_t* get_curr_page_dir();

uint32_t* page_table_alloc();

void page_fault_handler(context* context) {
    logger::panic("[PANIC] Page fault");
    while (1);
}

bool is_page_aligned(uint32_t address) {
    return address % PAGE_SIZE == 0;
}

uint32_t* get_page_table_address(uint32_t entry) { return (uint32_t*) (entry & 0xFFFFF000); }
uint32_t get_page_table_index(uint32_t virtual_address) { return virtual_address >> 12 & 0x03FF; }
bool is_present(uint32_t page_table) { return ((uint32_t) page_table) & (1 << 0); }
void set_present(uint32_t* entry) { (*entry) |= (1 << 0); }
void set_read_write(uint32_t* entry) { (*entry) |= (1 << 1); }
void unset_present(uint32_t* entry) { (*entry) &= ~(1 << 0); }

void map_page(uint32_t* page_directory, uint32_t virtual_address, uint32_t physical_address) {
    if (!is_page_aligned(virtual_address) || !is_page_aligned(physical_address)) {
        logger::panic("Trying to map addresses that are not page aligned!");
    }

    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_directory_entry = *(page_directory + page_directory_index);

    if (!is_present(page_directory_entry)) {
        // Page table is not present so we need to grab a new page table and put
        // it into the correct spot
        logger::msg_info("Page table not present for virtual address %x", virtual_address);
        logger::msg_info("Adding a new page table at index: %x", page_directory_index);

        uint32_t* new_page_table = page_table_alloc();
        logger::msg_info("New page table located at virtual address: %x", new_page_table);
        set_present((uint32_t*) &new_page_table);
        set_read_write((uint32_t*) &new_page_table);
        *(page_directory + page_directory_index) = (uint32_t) new_page_table;
    }

    uint32_t* page_table = get_page_table_address(page_directory_entry);
    uint32_t page_table_index = get_page_table_index(virtual_address);

    // if (is_present(page_table[page_table_index])) {
    //     logger::panic("Trying to map %x (phys) to %x (virt) but the virtual address is already mapped to something else", physical_address, virtual_address);
    // }

    uint32_t page_table_entry = physical_address;
    set_present(&page_table_entry);
    set_read_write(&page_table_entry);
    page_table[page_table_index] = page_table_entry;
}

void unmap_page(uint32_t* page_directory, uint32_t virtual_address) {
    // Check that the virtual address is page aligned
    if (!is_page_aligned(virtual_address)) {
        logger::panic("Trying to unmap a virtual address that is not page aligned!");
    }

    int page_directory_index = virtual_address >> 22;
    uint32_t page_directory_entry = *(page_directory + page_directory_index);

    // Make sure that the page directory entry is present
    if (!(is_present(page_directory_entry))) {
        logger::panic("Trying to unmap a virtual address that does not have a page directory entry!");
    }

    uint32_t* page_table = get_page_table_address(page_directory_entry);
    uint32_t page_table_index = get_page_table_index(virtual_address);

    // Unset the present bit
    uint32_t physical_address = page_table[page_table_index];
    if (!is_present(physical_address)) {
        logger::panic("Trying to unmap a virtual address that has not been mapped yet!");
    }
    unset_present(&physical_address);
    page_table[page_table_index] = physical_address;

    // TODO: maybe I can free the page table by scanning all the other
    // entries to see if they are present. Otherwise I may have to free
    // page tables only when the page directory is freed or in a GC style
}

/**
 * Unmap the initial mapping that goes from 0x0 to the virtual address of the
 * kernel end. This mapping was left by the boot stub and should be cleaned up
 * since we are already in the higher half.
 */
void unmap_bootstrap_mappings() {
    for (int i = 0; i < MEM_VIRT_KERNEL_END; i += PAGE_SIZE) {
        unmap_page(get_curr_page_dir(), i);
    }
}

//=============================================================================
// Page Frame Allocator
//=============================================================================

// Calculate the correct sized bitmap for our page frame allocator. Each bit
// represents one page.
const uint32_t PAGE_FRAME_ALLOCATOR_BITMAP_SIZE = (MEM_PHYS_PAGE_FRAME_AREA_START - MEM_PHYS_PAGE_FRAME_AREA_END) / PAGE_SIZE;
uint8_t page_frame_allocator_bitmap[PAGE_FRAME_ALLOCATOR_BITMAP_SIZE];

void init_page_frame_allocator() {
    // Make sure the allocator is page aligned
    if (!is_page_aligned(MEM_PHYS_PAGE_FRAME_AREA_START) ||
        !is_page_aligned(MEM_PHYS_PAGE_FRAME_AREA_END)) {
        logger::panic("Page frame allocator bounds are unaligned");
    }

    // Clear the bitmap (all pages are unmapped)
    memset(page_frame_allocator_bitmap, 0, sizeof(page_frame_allocator_bitmap));
}

uint32_t* page_frame_alloc(uint32_t virtual_address) {
    // Make sure the address is page aligned
    if (virtual_address % PAGE_SIZE != 0) {
        logger::panic("Trying to allocate a page fram at unaligned address!");
    }

    // Find a free page using the page_frame_bitmap.
    int32_t page_index = -1;
    for (int i = 0; i < sizeof(page_frame_allocator_bitmap); i++) {
        if (page_frame_allocator_bitmap[i] != UINT8_MAX) {
            // There is an available space so scan the bits
            for (int j = 0; j < UINT8_BITS; i++) {
                if (!(page_frame_allocator_bitmap[i] & (1 << j))) {
                    // We can allocate since the current bit is set to zero
                    page_index = i * UINT8_BITS + j;
                    page_frame_allocator_bitmap[i] |= (1 << j);
                    break;
                }
            }
            break;
        }
    }

    if (page_index == -1) {
        // There are no free pages left
        logger::panic("Page frame allocator has run out of space!");
    }

    // Calculate the physical address and map it into memory
    uint32_t physical_address = MEM_PHYS_PAGE_FRAME_AREA_START + (PAGE_SIZE * page_index);
    map_page(get_curr_page_dir(), virtual_address, physical_address);

    return (uint32_t*) virtual_address;
}

//=============================================================================
// Page Table Allocator
//=============================================================================

// Calculate the correct sized bitmap for the page table allocator. Each bit
// represents one page.
const uint32_t PAGE_TABLE_ALLOCATOR_BITMAP_SIZE = (MEM_PHYS_PAGE_TABLE_RESERVED_AREA_END - MEM_PHYS_PAGE_TABLE_RESERVED_AREA_START) / PAGE_SIZE;
uint8_t page_table_allocator_bitmap[PAGE_TABLE_ALLOCATOR_BITMAP_SIZE];

/**
 * Map the page table reserved area at a known offset. This is important because
 * otherwise we may deadlock trying to allocate a page frame. The reason for
 * this deadlock is that a page frame allocation request may need to also
 * allocate a page table. However, if we don't pre-map the page tables then we
 * can have a situation in which we try to allocate a page but need a page table
 * but allocating that page table requires allocating a page.
 */
void init_page_table_allocator() {
    uint32_t test = (uint32_t) get_curr_page_dir(); 

    // Check that the constants in the memory map are sane for the page table
    // allocator. The size of physical memory must be the same as the size of
    // virtual memory and all of the constants must be page aligned.
    if (!is_page_aligned(MEM_PHYS_PAGE_TABLE_RESERVED_AREA_END) ||
        !is_page_aligned(MEM_PHYS_PAGE_TABLE_RESERVED_AREA_START) ||
        !is_page_aligned(MEM_VIRT_PAGE_TABLE_RESERVED_AREA_END) ||
        !is_page_aligned(MEM_VIRT_PAGE_TABLE_RESERVED_AREA_START))
    {
        logger::panic("The page table reserved area is not page aligned!");
    }
    uint32_t physical_size = MEM_PHYS_PAGE_TABLE_RESERVED_AREA_END - MEM_PHYS_PAGE_TABLE_RESERVED_AREA_START;
    uint32_t virtual_size = MEM_VIRT_PAGE_TABLE_RESERVED_AREA_END - MEM_VIRT_PAGE_TABLE_RESERVED_AREA_START;
    if (physical_size != virtual_size) {
        logger::panic("The page table reserved area is an incompatible size!");
    }

    // Do a little bit of magic here to make things work. We need to put the
    // static page table into the page directory at the correct address to map
    // in all of our reserved page tables. A simple chicken and egg problem
    // because we may (will) need to allocate a page table in order to map the
    // memory that will contain the reserved page table. To get around this we
    // use a single static page table. As long as the arena for reserved page
    // tables stays under 4Mib we are okay. Otherwise we will need to repeat
    // this action with a second static page table.
    uint32_t page_directory_index = MEM_VIRT_PAGE_TABLE_RESERVED_AREA_START >> 22;
    uint32_t page_directory_entry = *(get_curr_page_dir() + page_directory_index);
    uint32_t* new_page_table = ptr_page_table;
    set_present((uint32_t*) &new_page_table);
    set_read_write((uint32_t*) &new_page_table);
    *(get_curr_page_dir() + page_directory_index) = (uint32_t) new_page_table;

    // Map the physical reserved area into the virtual reserved area
    for (int i = 0; i < virtual_size; i += PAGE_SIZE) {
        map_page(get_curr_page_dir(), MEM_VIRT_PAGE_TABLE_RESERVED_AREA_START + i, MEM_PHYS_PAGE_TABLE_RESERVED_AREA_START + i);
    }
}

uint32_t* page_table_alloc() {
    // Find a free page using the page table bitmap
    int32_t index = -1;
    for (int i = 0; i < sizeof(page_table_allocator_bitmap); i++) {
        if (page_table_allocator_bitmap[i] != UINT8_MAX) {
            // There is an available space so scan the bits
            for (int j = 0; j < UINT8_BITS; i++) {
                if (!(page_table_allocator_bitmap[i] & (1 << j))) {
                    // We can allocate since the current bit is set to zero
                    index = i * UINT8_BITS + j;
                    page_table_allocator_bitmap[i] |= (1 << j);
                    break;
                }
            }
            break;
        }
    }

    if (index == -1) {
        // There are no free page tables left
        logger::panic("Trying to allocate page table but no page tables left to allocate!");
    }

    // Since we know that the entire page table reserved region is already
    // mapped we can just return the virtual address without checking whether or
    // not it is valid (we assume that it is).
    const uint32_t virtual_address = MEM_VIRT_PAGE_TABLE_RESERVED_AREA_START + (PAGE_SIZE * index);
    return (uint32_t*) virtual_address;
}


void init() {
    unmap_bootstrap_mappings();
    init_page_frame_allocator();
    init_page_table_allocator();
}

} // namespace memory