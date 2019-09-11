#include "cstring.hpp"

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void strrev(char * str) {
    size_t len = strlen(str);
    for (int i = 0; i < len/2; i++) {
        char temp = str[i];
        // swap characters
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}

void* memset(void* bufptr, int value, size_t size) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}