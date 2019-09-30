#include "memory.hpp"
#include "kernel/logger/logger.hpp"
#include "system/isr.hpp"
#include "lib/cstring.hpp"


namespace memory {

extern "C" void load_page_dir(uint32_t);
extern "C" void enable_paging();
extern "C" void load_page_dir(uint32_t);
extern "C" uint32_t get_cr3();

uint32_t* page_table_alloc();
uint32_t reserve_physical_frame();

/**
 * Only valid after the recursive page mappings have been initialized.
 */ 
uint32_t* get_curr_page_dir() {
    return (uint32_t*) 0xFFFFF000;
}

/**
 * Returns the virtual address of page table that exists at
 * page_directory[page_directory_index]. Note that this address is not
 * guarenteed to be mapped. It is the caller's responsibility to ensure that
 * this address actually has a page table and is mapped into memory.
 */
uint32_t* get_page_table_virt(uint32_t page_directory_index) {
    uint32_t ret = 0xFFC00000;  // Use the recursive mapping
    ret |= page_directory_index << 12;
    return (uint32_t*) ret;
}

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

uint32_t get_physical_address(uint32_t virtual_address) {
    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_table_index = virtual_address >> 12 & 0x03FF;
    return 0;
    // TODO: finish this
}

void map_page(uint32_t* page_directory, uint32_t virtual_address, uint32_t physical_address) {
    if (!is_page_aligned(virtual_address) || !is_page_aligned(physical_address)) {
        logger::panic("Trying to map addresses that are not page aligned!");
    }

    uint32_t page_directory_index = virtual_address >> 22;
    uint32_t page_directory_entry = page_directory[page_directory_index];

    if (!is_present(page_directory_entry)) {
        // Page table is not present so we need to grab a new page table and put
        // it into the correct spot
        uint32_t new_page_table_phys = reserve_physical_frame();
        set_present(&new_page_table_phys);
        set_read_write(&new_page_table_phys);
        page_directory[page_directory_index] = new_page_table_phys;
    }

    // Use the recursive page mapping to get the virtual address of the page table
    uint32_t* page_table = get_page_table_virt(page_directory_index);
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
    uint32_t test = (uint32_t) page_directory;

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
    for (int i = 0; i < MEM_PHYS_KERNEL_END; i += PAGE_SIZE) {
        unmap_page((uint32_t*) get_curr_page_dir(), i);
    }
}

/**
 * Setup a recursive mapping in the page directory. This scheme is known as
 * recursive (or fractal) page mapping. By mapping the last entry of the page
 * directory to itself we can map certain high virtual addresses to the page
 * tables so that we can actually access them and change them without knowing
 * their virtual addresses beforehand.
 */ 
void setup_recursive_mapping() {
    // Cannot use get_cur_dir() because that assumes that recursive mappings have already been initialized.
    uint32_t* page_directory_virt = (uint32_t*) (get_cr3() + MEM_PHYS_KERNEL_BASE);

    uint32_t page_directory_phys = get_cr3();
    set_present(&page_directory_phys);
    set_read_write(&page_directory_phys);
    page_directory_virt[1024 - 1] = page_directory_phys;
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

/**
 * Reserves a page frame in physical memory by consulting the page frame
 * allocator bitmap. Returns the physical address of the reserved frame. Mapping
 * this frame into memory is the responsibility of the caller.
 */
uint32_t reserve_physical_frame() {
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
    return physical_address;
}

uint32_t* page_frame_alloc(uint32_t virtual_address) {
    // Make sure the address is page aligned
    if (virtual_address % PAGE_SIZE != 0) {
        logger::panic("Trying to allocate a page fram at unaligned address!");
    }

    uint32_t physical_address = reserve_physical_frame();
    map_page(get_curr_page_dir(), virtual_address, physical_address);

    return (uint32_t*) virtual_address;
}

//=============================================================================
// Heap Allocator
//=============================================================================

// Both the current location of the top of the heap and the mapped pointer
// should start at the same location since nothing has been allocated yet.
uint32_t heap_mapped = MEM_VIRT_KHEAP_START;
uint32_t heap_current = MEM_VIRT_KHEAP_START;

void init_heap_allocator() {
    // Ensure the heap allocator is page aligned
    if (!is_page_aligned(MEM_VIRT_KHEAP_START) || !is_page_aligned(MEM_VIRT_KHEAP_END)) {
        logger::panic("The heap allocator is not page aligned!");
    }
}

#define heap_allocate(a) halloc(a)
void* halloc(size_t size) {
    logger::msg_info("Heap allocation of size %u bytes at address %x", size, heap_current);
    // Check that the heap does not overflow
    if (heap_current + size > MEM_VIRT_KHEAP_END) {
        logger::panic("Heap is overflowing! Cannot allocate!");
    }

    // Allocate new pages for the heap lazily when the current point passes the
    // furthest mapped address
    while(heap_current + size > heap_mapped) {
        page_frame_alloc(heap_mapped);
        heap_mapped += PAGE_SIZE;
    }

    // Clear the memory before allocating
    memset((void*) heap_current, 0, size);

    uint32_t ret = heap_current;
    heap_current += size;
    return (void*) ret;
}

void free(void* m) {
    // Don't need it yet ;)
    return;
}

void init() {
    setup_recursive_mapping();
    unmap_bootstrap_mappings();
    init_heap_allocator();

    halloc(256);
    halloc(16);
    halloc(16);
    halloc(16);
}

} // namespace memory