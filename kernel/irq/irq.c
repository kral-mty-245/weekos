// WeeK OS - IRQ Handler
// Manages hardware interrupts

#include "../../include/kernel.h"

#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1

// IRQ handlers
typedef void (*irq_handler_t)(void);
static irq_handler_t irq_handlers[16] = {0};

// Remap PIC
static void pic_remap(void) {
    // ICW1: begin initialization
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);
    
    // ICW2: vector offset
    outb(PIC1_DATA, 0x20);  // IRQ 0-7 -> INT 32-39
    outb(PIC2_DATA, 0x28);  // IRQ 8-15 -> INT 40-47
    
    // ICW3: cascading
    outb(PIC1_DATA, 0x04);  // Slave at IRQ2
    outb(PIC2_DATA, 0x02);
    
    // ICW4: 8086 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // Mask all IRQs
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

// Initialize IRQ system
void irq_init(void) {
    pic_remap();
    
    // Enable IRQ0 (timer), IRQ1 (keyboard), IRQ12 (mouse)
    irq_unmask(0);   // Timer
    irq_unmask(1);   // Keyboard
    irq_unmask(12);  // Mouse
}

// Unmask an IRQ
void irq_unmask(uint8_t irq) {
    if (irq < 8) {
        outb(PIC1_DATA, inb(PIC1_DATA) & ~(1 << irq));
    } else {
        outb(PIC2_DATA, inb(PIC2_DATA) & ~(1 << (irq - 8)));
    }
}

// Mask an IRQ
void irq_mask(uint8_t irq) {
    if (irq < 8) {
        outb(PIC1_DATA, inb(PIC1_DATA) | (1 << irq));
    } else {
        outb(PIC2_DATA, inb(PIC2_DATA) | (1 << (irq - 8)));
    }
}

// Send EOI (End of Interrupt)
void irq_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, 0x20);
    }
    outb(PIC1_CMD, 0x20);
}

// Register IRQ handler
void irq_register(uint8_t irq, irq_handler_t handler) {
    irq_handlers[irq] = handler;
}

// IRQ dispatch
void irq_handler(registers_t* regs) {
    uint8_t irq = regs->int_no - 32;
    
    // Call handler
    if (irq_handlers[irq]) {
        irq_handlers[irq]();
    }
    
    // Send EOI
    irq_eoi(irq);
}
