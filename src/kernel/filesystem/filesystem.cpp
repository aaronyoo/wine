#include "filesystem.hpp"

namespace filesystem {

// Multiboot module looks like this:
//         +-------------------+
// 0       | mod_start         |
// 4       | mod_end           |
//         +-------------------+
// 8       | string            |
//         +-------------------+
// 12      | reserved (0)      |
//         +-------------------+

void init(multiboot_info_t* minfo) {
    uint32_t initrd_start = (uint32_t) minfo->mods_addr;
    uint32_t initrd_end = (uint32_t) (minfo->mods_addr + 4);

    logger::msg_info("Initrd START: %x", initrd_start);
    logger::msg_info("Initrd END: %x", initrd_end);
}

}