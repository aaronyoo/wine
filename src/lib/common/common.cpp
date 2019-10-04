#include "common.hpp"
#include <stddef.h>
#include "kernel/memory/memory.hpp"
#include "kernel/logger/logger.hpp"

void* operator new(size_t size) {
    void* m = memory::halloc(size);
    logger::msg_info("User implemented NEW triggered!");
    return m;
}

void operator delete(void* m) {
    logger::msg_info("User implemented DELETE triggered");
    memory::free(m);
}

void* operator new[](size_t size) {
    logger::msg_info("User implemented NEW[] triggered");
    void* m = memory::halloc(size);
    return m;
}

void operator delete[](void* m) {
    logger::msg_info("User implemented DELETE[] triggered");
    memory::free(m);
}