#ifndef CSTRING_HPP
#define CSTRING_HPP

#include <stddef.h>

size_t strlen(const char* str);
void strrev(char * str);
void* memset(void* bufptr, int value, size_t size);

#endif // CSTRING_HPP