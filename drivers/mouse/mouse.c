// WeeK OS - Mouse Driver (PS/2)
// Handles mouse input

#include "../../include/kernel.h"

#define MOUSE_DATA_PORT    0x60
#define MOUSE_STATUS_PORT  0x64
#define MOUSE_CMD_PORT     0x64

// Mouse state
static int32_t mouse_x = 400;
static int32_t mouse_y = 300;
static uint8_t mouse_buttons = 0;
static uint8_t mouse_cycle = 0;
static int8_t mouse_byte[3];

// Mouse event buffer
#define MOUSE_BUFFER_SIZE 64
static mouse_event_t mouse_buffer[MOUSE_BUFFER_SIZE];
static int mouse_head = 0;
static int mouse_tail = 0;

// Push mouse event
static void mouse_push_event(mouse_event_t* event) {
    int next = (mouse_head + 1) % MOUSE_BUFFER_SIZE;
    if (next != mouse_tail) {
        mouse_buffer[mouse_head] = *event;
        mouse_head = next;
    }
}

// Pop mouse event
int mouse_pop_event(mouse_event_t* event) {
    if (mouse_head == mouse_tail) return 0;
    *event = mouse_buffer[mouse_tail];
    mouse_tail = (mouse_tail + 1) % MOUSE_BUFFER_SIZE;
    return 1;
}

// Mouse interrupt handler (IRQ12)
static void mouse_irq_handler(void) {
    uint8_t data = inb(MOUSE_DATA_PORT);
    
    // Process mouse packet
    mouse_byte[mouse_cycle] = data;
    mouse_cycle++;
    
    if (mouse_cycle == 3) {
        mouse_cycle = 0;
        
        // Parse mouse packet
        int8_t dx = mouse_byte[1];
        int8_t dy = mouse_byte[2];
        uint8_t buttons = mouse_byte[0];
        
        // Update position
        mouse_x += dx;
        mouse_y -= dy;
        
        // Clamp to screen
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_x >= 1920) mouse_x = 1919;
        if (mouse_y >= 1080) mouse_y = 1079;
        
        // Update buttons
        mouse_buttons = buttons;
        
        // Create and push event
        mouse_event_t event;
        event.x = mouse_x;
        event.y = mouse_y;
        event.left_button = buttons & 0x01;
        event.right_button = (buttons >> 1) & 0x01;
        event.middle_button = (buttons >> 2) & 0x01;
        
        mouse_push_event(&event);
    }
}

// Send mouse command
static void mouse_send_cmd(uint8_t cmd) {
    // Wait for controller
    while (inb(MOUSE_STATUS_PORT) & 0x02);
    outb(MOUSE_CMD_PORT, 0xD4);
    
    // Wait for controller
    while (inb(MOUSE_STATUS_PORT) & 0x02);
    outb(MOUSE_DATA_PORT, cmd);
}

// Initialize mouse
void mouse_init(void) {
    // Enable auxiliary device (mouse)
    while (inb(MOUSE_STATUS_PORT) & 0x02);
    outb(MOUSE_CMD_PORT, 0xA8);
    
    // Enable IRQ12
    while (inb(MOUSE_STATUS_PORT) & 0x02);
    outb(MOUSE_CMD_PORT, 0x20);
    
    uint8_t status = inb(MOUSE_DATA_PORT);
    status |= 0x02;  // Enable IRQ12
    status &= ~0x20; // Enable mouse
    while (inb(MOUSE_STATUS_PORT) & 0x02);
    outb(MOUSE_CMD_PORT, 0x60);
    while (inb(MOUSE_STATUS_PORT) & 0x02);
    outb(MOUSE_DATA_PORT, status);
    
    // Set mouse defaults
    mouse_send_cmd(0xF6);
    mouse_send_cmd(0xF4);
    
    // Register IRQ12 handler
    irq_register(12, mouse_irq_handler);
    
    // Unmask mouse IRQ
    irq_unmask(12);
}

// Get mouse position
int32_t mouse_get_x(void) { return mouse_x; }
int32_t mouse_get_y(void) { return mouse_y; }
uint8_t mouse_get_buttons(void) { return mouse_buttons; }
