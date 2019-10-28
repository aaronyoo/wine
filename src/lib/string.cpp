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
    memory::free(value);
    size = strlen(rhs.value);
    value = (char*) memory::halloc(size);
    memcpy(value, rhs.value, size);
    // TODO: it seems unclear if I should free the rhs help me
    return *this;
}


char* String::get_value() {
    return value;
}