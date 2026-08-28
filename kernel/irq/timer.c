// WeeK OS - Programmable Interval Timer (PIT)
// Timer for system clock and scheduling

#include "../../include/kernel.h"

#define PIT_FREQ    1193180  // Base frequency
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43

static uint32_t timer_ticks = 0;
static uint32_t timer_freq = 0;

// Initialize PIT
void timer_init(uint32_t frequency) {
    timer_freq = frequency;
    
    // Calculate divisor
    uint32_t divisor = PIT_FREQ / frequency;
    
    // Send command byte
    outb(PIT_COMMAND, 0x36);  // Channel 0, lobyte/hibyte, rate generator
    
    // Send divisor
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

// Timer interrupt handler (called by IRQ0)
void timer_handler(void) {
    timer_ticks++;
    
    // Call scheduler
    scheduler_tick();
}

// Get current tick count
uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

// Sleep for specified milliseconds
void sleep(uint32_t ms) {
    uint32_t target = timer_ticks + (ms * timer_freq / 1000);
    while (timer_ticks < target) {
        asm volatile("hlt");  // Wait for next interrupt
    }
}

// Get current time in seconds
uint32_t timer_get_seconds(void) {
    return timer_ticks / timer_freq;
}
