#ifndef TASKING_HPP
#define TASKING_HPP

#include <stdint.h>
#include "lib/string.hpp"
#include "lib/common/common.hpp"

namespace tasking {

#define MAX_PROCESS_NAME_LENGTH 20

// Definitions for process states
#define RUNNING 0
#define READY_TO_RUN 1
#define BLOCKED 2
#define TERMINATED 3

// TODO: Possibly add back in later but right now we don't need
//       to really save any registers.
// typedef struct {
//     uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip;
// } registers_t;

class Process {
public:
    Process(const char* n)
        : task_name(n) 
    {}
    Process() 
        : task_name() 
    {}

    uint32_t pid;
    String task_name;

    uint32_t* kernel_stack_top;
    uint32_t page_directory;
    uint32_t state;
    uint32_t time_used;
};

void init();

}  // namespace tasking

#endif // TASKING_HPP