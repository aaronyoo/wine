#include "logger.hpp"
#include "system/serial_port.hpp"
#include "lib/cstring.hpp"
#include <stdarg.h>

namespace logger {

extern "C" void print_hex(const uint32_t n);

void print_serial_unsigned_decimal(uint32_t n) {
    // TODO: is there a better solution to this case??
    // I am assuming that there is only 15 digits in the worst case
    // for an unsigned decimal number of size uint32_t.
    char s[25];
    int pos = 0;

    // Treat 0 as a special case
    if (n == 0) {
        s[0] = '0';
        s[1] = '\0';
        serial_port::print(s);
        return;
    }

    while (n > 0) {
        s[pos] = (n % 10) + '0'; // convert to ascii
        n /= 10;
        pos++;
    }
    s[pos] = '\0';

    // now the number will be reversed so we need to reverse it again
    strrev(s);
    serial_port::print(s);
}

void init() {
    // Logger uses the COM1 serial port
    serial_port::init(COM1_BASE);
}

void msg_info(const char* format, ...) {
    va_list args;
    va_start(args, format);

    while (*format != '\0') {
        if (format[0] != '%') {
            serial_port::putc(format[0]);
            format++;
            continue;
        }

        format++;  // skip past '%'

        if (*format == 's') { // string
            format++;
            const char* str = va_arg(args, const char*);
            size_t len = strlen(str);
            serial_port::print(str);
        } else if (*format == 'c') { // character
            format++;
            char c = (char) va_arg(args, int); // char is promoted to int
            serial_port::putc(c);
        } else if (*format == 'x') { // hex number
            format++;
            uint32_t num = va_arg(args, uint32_t);
            serial_port::putc('0');
            serial_port::putc('x');
            print_hex(num);
        } else if (*format == 'u') { // unsigned decimal
            format++;
            uint32_t num = va_arg(args, uint32_t);
            logger::print_serial_unsigned_decimal(num);
        }
    }

    va_end(args);
    serial_port::putc('\n');
}

void panic(const char* format, ...) {
    va_list args;
    va_start(args, format);
    msg_info(format, args);
    va_end(args);

    while (1); // hang
}

} // namespace logger