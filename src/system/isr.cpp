#include "isr.hpp"
#include "pic.hpp"
#include "kernel/logger/logger.hpp"

isr_t interrupt_handlers[256];

namespace isr {

extern "C" void isr_handler(context_t* context) {
    logger::msg_info("Received interrupt: %u\n", context->int_no);

    if (interrupt_handlers[context->int_no] != 0) {
        isr_t handler = interrupt_handlers[context->int_no];
        handler(context);
    }
}

extern "C" void irq_handler(context_t* context) {
    // Send an EOI to the PIC-> 
    // Subtract 32 from the interrupt number to get the IRQ number->
    pic::send_eoi(context->int_no - 32);

    if (interrupt_handlers[context->int_no] == 0) {
        logger::msg_info("[PANIC] Unhandled IRQ: %u\n", context->int_no - 32);
        while(1); // TODO: change this to panic
    }

    isr_t handler = interrupt_handlers[context->int_no];
    handler(context);
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    logger::msg_info("Registering interrupt handler for interrupt %u\n", n);
    interrupt_handlers[n] = handler;
}

} // namespace isr