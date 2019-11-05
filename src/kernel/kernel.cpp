#include "kernel/logger/logger.hpp"
#include "kernel/memory/memory.hpp"
#include "system/gdt.hpp"
#include "system/pic.hpp"
#include "system/idt.hpp"
#include "lib/common/common.hpp"
#include "kernel/multiboot.hpp"
#include "kernel/tasking/tasking.hpp"

namespace kernel {

void system_setup(multiboot_info_t* minfo, uint32_t magic) {
    gdt::init();
    pic::init();
    idt::init();

    logger::msg_info("===============");
    logger::msg_info("MULTIBOOT INFO");
    logger::msg_info("===============");
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        logger::msg_info("[Error] Multiboot magic: %x", magic);
        while (1); // hang
    }

    logger::msg_info("MULTIBOOT FLAGS: %x", minfo->flags);
    if (minfo->flags & MULTIBOOT_INFO_MEMORY) {
        logger::msg_info("MEM_LOWER = %x", minfo->mem_lower);
        logger::msg_info("MEM_UPPER = %x", minfo->mem_upper);
    }

    logger::msg_info("");
    logger::msg_info("MEMORY MAP:");
    if (minfo->flags & MULTIBOOT_INFO_MEM_MAP) {
        multiboot_memory_map_t* map;

        for (map = (multiboot_memory_map_t*) minfo->mmap_addr;
            (uint32_t) map < (minfo->mmap_addr + minfo->mmap_length);
            map = (multiboot_memory_map_t*)((uint32_t) map + map->size + sizeof(map->size))) 
        {
            logger::msg_info("\tBASE ADDRESS = %x", map->addr_lower); 
            logger::msg_info("\tLENGTH = %x", map->len_lower);
            logger::msg_info("\tTYPE = %x", map->type);
            logger::msg_info("");
        }
    }

    // Check for the ramdisk module
    if (minfo->mods_count <= 0) {
        logger::msg_info("Modules: None");
    }
}

extern "C" void main(multiboot_info_t* minfo, uint32_t magic) {
    // Initialize logger first to provide the ability to generate debug output
    // for the rest of execution
    logger::init();

    kernel::system_setup(minfo, magic);

    // Initialize memory ASAP so that other modules can use new() and free()
    // After this call, all modules can use a user implemented new() and free()
    memory::init();

    // Initialize tasking so that we can get process primitives
    tasking::init();

    __asm__ volatile("sti");

    while(1) {

    }
    
}

} // namespace kernel