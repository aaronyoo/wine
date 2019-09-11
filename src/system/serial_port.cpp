#include "serial_port.hpp"
#include "lib/cstring.hpp"
#include "system.hpp"

int is_transmit_empty() {
   return inb(COM1_BASE + 5) & 0x20;
}

void put_serial(char c) {
   while (is_transmit_empty() == 0);   // Stall while transit is filled

   outb(COM1_BASE,c);
}

void print_serial(const char *s) {
    for (size_t i = 0; i < strlen(s); i++) {
        put_serial(s[i]);
    }
}

void serial_port::init(uint32_t base) {
    outb(base + 1, 0x00);    // Disable all interrupts
    outb(base + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(base + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(base + 1, 0x00);    //                  (hi byte)
    outb(base + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(base + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(base + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void serial_port::print(const char *s) {
    print_serial(s);
}

void serial_port::putc(const char c) {
    put_serial(c);
}