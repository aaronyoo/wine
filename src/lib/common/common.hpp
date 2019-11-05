#ifndef COMMON_HPP
#define COMMON_HPP

#include <stddef.h>
#include <stdint.h>
#include "kernel/memory/memory.hpp"
#include "kernel/logger/logger.hpp"
#include "lib/panic.hpp"

void* operator new(size_t size);
void operator delete(void* m);
void* operator new[](size_t size);
void operator delete[](void* m);

#endif  // COMMON_HPP