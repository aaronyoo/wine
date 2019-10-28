#include "kernel/tasking/tasking.hpp"
#include "lib/list.hpp"
#include "lib/common/common.hpp"
#include "kernel/memory/memory.hpp"
#include "lib/string.hpp"

extern "C" void switch_to_task_stub(tasking::Process* p);
tasking::Process* current_task;


namespace tasking {

List<Process> ready;
List<Process> blocked;
List<Process> waiting;
List<Process> terminated;

uint32_t next_available_pid;

void task1();
void task2();
Process* create_kernel_task(const char* name, void (*start_eip)(void));
void rotate_task();
Process* deqeue_task(List<Process>& queue);
void enqueue_task(const Process& proc, List<Process>& queue);
void switch_to_task(Process* p);


void init() {
    // Create the init process
    Process proc("test");
    logger::msg_info(proc.task_name.get_value());

    ready.append(proc);

    logger::msg_info("%u", offsetof(Process, kernel_stack_top));
    logger::msg_info("%u", offsetof(Process, page_directory));

    Process* t1 = create_kernel_task("task1", task1);
    Process* t2 = create_kernel_task("task2", task2);
    enqueue_task(*t1, ready);
    enqueue_task(*t2, ready);
    rotate_task(); // rotate away from the main kernel thread
}

// 
void switch_to_task(Process* p) {
    switch_to_task_stub(current_task);
    current_task = p;
}

// Enqueues a task to the specified queue.
void enqueue_task(const Process& proc, List<Process>& queue) {
    queue.append(proc);
}

// Dequeues a task from the specified queue
Process* deqeue_task(List<Process>& queue) {
    queue.dequeue();
}

// Takes the next task that is ready and runs it.
// Places the current_task task on the back of the ready queue.
void rotate_task() {
    if (ready.size() > 0) {
        enqueue_task(*current_task, ready);
        switch_to_task(deqeue_task(ready));
        return;
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
    task->kernel_stack_top = stack_area + 1024;  // Since the stack grows downward

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
        logger::msg_info("1");
        rotate_task();
    }
}

void task2() {
    while(1) {
        logger::msg_info("2");
        rotate_task();
    }
}

}  // namespace tasking
