#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include "kernel/multiboot.hpp"
#include "lib/common/common.hpp"

namespace filesystem {

void init(multiboot_info_t* minfo);

}

#endif  // FILESYSTEM_HPP