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

    Process* proc = new Process("test");
    logger::msg_info(proc->task_name.get_value());


    // Code below this is just to test the list implementation
    // Should be removed TODO
    List<int> test;
    test.insert(test.begin(), 1);
    test.append(2);
    test.append(3);
    test.append(4);
    test.insert(test.begin(), 5);

    auto testit = test.begin();
    testit++;
    test.insert(testit, 6);

    for (auto it = test.begin(); it != test.end(); it++) {
        logger::msg_info("%u", *it);
    }

    logger::msg_info("Test size: %u", test.size());
}

}  // namespace tasking
