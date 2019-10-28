#include "kernel/tasking/tasking.hpp"
#include "lib/list.hpp"
#include "lib/common/common.hpp"
#include "kernel/memory/memory.hpp"
#include "lib/string.hpp"
#include "lib/cstring.hpp"

extern "C" void switch_to_task_stub(tasking::Process* p);
tasking::Process* previous_task;

namespace tasking {

List<Process*> ready;
List<Process*> blocked;
List<Process*> waiting;
List<Process*> terminated;

uint32_t next_available_pid;
Process* current_task;

void task1();
void task2();
Process* create_kernel_task(const char* name, void (*start_eip)(void));
void schedule();

Process* t1;
Process* t2;

// 
void switch_to_task(Process* p) {
    previous_task = current_task;
    current_task = p;
    switch_to_task_stub(p);
}

// Enqueues a task to the specified queue.
void enqueue_task(Process* proc, List<Process*>& queue) {
    queue.append(proc);
}

// Dequeues a task from the specified queue
Process* dequeue_task(List<Process*>& queue) {
    return queue.dequeue();
}

// Takes the next task that is ready and runs it.
// Places the current_task task on the back of the ready queue.
void schedule() {
    if (ready.size() > 0) {
        enqueue_task(current_task, ready);
        Process* next_task = dequeue_task(ready);
        switch_to_task(next_task);
    }
    // Else, there is an empty ready queue so we might as well just keep running the same task
}

// Creates a kernel task given a name and a function pointer. Having NULL as a
// function pointer signals that there is no stack setup needed. NULL should
// only really be used for the initial kernel thread.
Process* create_kernel_task(const char* name, void (*start_eip)(void)) {
    Process* task = new Process(name);

    // Make the stack area a fixed size for now, we may have to change this
    // later but it should be okay for now since there should only be around 1
    // stack frame of stuff on the kernel stack at any one time.
    uint32_t* stack_area = (uint32_t*) memory::halloc(4096);
    task->kernel_stack_top = stack_area + 1023;  // Since the stack grows downward we want to move it to 1023 * 4 < 4096

    // The new task will have the same page directory as the creating task
    task->page_directory = (uint32_t) memory::get_curr_page_dir();

    // The new task is ready to run by default
    task->state = READY;

    // Make sure that the task is in a state that it can be switched into. This
    // means putting eip on the top of the kernel stack and decrementing the top
    // of the stack for the registers that are pushed during the task switch
    // stub.
    if (start_eip != NULL) {
        *(task->kernel_stack_top) = (uint32_t) start_eip;
        task->kernel_stack_top -= 4;  // decrement for ebp edi esi ebx
    }

    return task;
}

//=======================================
// Test Functions -- TODO: delete later
//=======================================
void task1() {
    while(1) {
        logger::msg_info("111111111111111");
        schedule();
    }
}

void task2() {
    while(1) {
        logger::msg_info("222222222222222");
        schedule();
    }
}

void init() {

    current_task = create_kernel_task("kernel", NULL);
    t1 = create_kernel_task("task1", task1);
    t2 = create_kernel_task("task2", task2);

    enqueue_task(t1, ready);
    enqueue_task(t2, ready);

    schedule(); // rotate away from the main kernel thread
    int i = 3;
    while(i--) {
        logger::msg_info("kernelkernelkernelkernel");
        schedule();
    }
}

}  // namespace tasking
