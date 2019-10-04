#ifndef STRING_HPP
#define STRING_HPP

#include "lib/cstring.hpp"
#include "kernel/memory/memory.hpp"

class String {
public:
    String(const char* s);
    String();
    ~String();

    String& operator=(const String& rhs);
    char* get_value();

private:
    uint32_t size;
    char* value;
};

#endif  // STRING_HPP