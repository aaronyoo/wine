#include "memory.hpp"
#include "kernel/logger/logger.hpp"
#include "system/isr.hpp"

namespace memory {

extern "C" void load_page_dir(uint32_t);
extern "C" void enable_paging();

page_dir_t get_curr_page_dir() {
    page_dir_t current;
    asm volatile("mov %%cr3, %0" : "=r"(current));
    return current;
}

// phys_addr_t get_phys_addr(virt_addr_t va) {
//     uint32_t pd_idx = ((uint32_t) va) >> 22;
//     uint32_t pt_idx = ((uint32_t) va) >> 12 & 0x03FF

//     page_dir_t = get_curr_page_dir();

// }

void page_fault_handler(context* context) {
    logger::msg_info("[PANIC] Page fault");
    while (1);
}

void init() {
    // Check that boot code left us in a sane state
    page_dir_t curr_page_dir = get_curr_page_dir();
    
    // page_table_t 
}

} // namespace memory