#ifndef PIC_H
#define PIC_H

namespace pic {
    void init();
    void send_eoi(unsigned char irq);
}

#endif // PIC_H