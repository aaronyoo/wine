#include "panic.hpp"

void panic(const char* msg) {
    logger::msg_info("[PANIC]: %s", msg);
    while (1);  // hang forever
}