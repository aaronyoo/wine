#ifndef CSTRING_HPP
#define CSTRING_HPP

#include <stddef.h>

size_t strlen(const char* str);
void strrev(char * str);
void* memset(void* bufptr, int value, size_t size);
int memcmp(const void* s1, const void* s2,size_t n);
void * memcpy (void *dest, const void *src, size_t len);

#endif // CSTRING_HPP