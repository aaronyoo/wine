#include "kernel/tasking/tasking.hpp"
#include "lib/list.hpp"
#include "lib/common/common.hpp"
#include "kernel/memory/memory.hpp"
#include "lib/string.hpp"

namespace tasking {

List<Process>* ready;
List<Process>* blocked;
List<Process>* waiting;
List<Process>* terminated;

uint32_t next_available_pid;

void init() {
    ready = new List<Process>();
    blocked = new List<Process>();
    waiting = new List<Process>();

    List<int>* test = new List<int>();
    Process* proc = new Process("test");
    logger::msg_info(proc->task_name.get_value());

}

}  // namespace tasking
