#include "logger/logger.hpp"
#include "system/gdt.hpp"
#include "system/pic.hpp"
#include "system/idt.hpp"
#include "multiboot.hpp"

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
    logger::init();
    kernel::system_setup(minfo, magic);



    __asm__ volatile("sti");

    while(1) {

    }
    
}

} // namespace kernel