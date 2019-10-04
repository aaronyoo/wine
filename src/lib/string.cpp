#include "string.hpp"

String::String(const char* s) {
    size = strlen(s);
    value = (char*) memory::halloc(size);
    memcpy(value, s, size);
}

String::String() {
    size = 0;
}

String::~String() {
    memory::free(value);
}

String& String::operator=(const String& rhs) {
    // TODO: it seems unclear if I should free the rhs
}


char* String::get_value() {
    return value;
}