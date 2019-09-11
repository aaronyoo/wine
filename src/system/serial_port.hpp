#ifndef SERIAL_PORT_HPP
#define SERIAL_PORT_HPP

#include <stdint.h>

#define COM1_BASE 0x3f8   /* COM1 */

namespace serial_port {
    void init(uint32_t base);
    void print(const char *s);
    extern "C" void putc(const char c);
}

#endif // SERIAL_PORT_HPP