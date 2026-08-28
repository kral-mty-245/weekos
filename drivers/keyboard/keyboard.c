// WeeK OS - Keyboard Driver (PS/2)
// Handles keyboard input

#include "../../include/kernel.h"

#define KB_DATA_PORT    0x60
#define KB_STATUS_PORT  0x64

// Key states
static uint8_t shift_pressed = 0;
static uint8_t ctrl_pressed = 0;
static uint8_t alt_pressed = 0;
static uint8_t caps_lock = 0;

// US QWERTY keymap
static const char keymap[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']', '\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',
    0,'*', 0,' '
};

static const char keymap_shift[] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}', '\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',
    0,'|','Z','X','C','V','B','N','M','<','>','?',
    0,'*', 0,' '
};

// Key event buffer
#define KB_BUFFER_SIZE 256
static keyboard_event_t kb_buffer[KB_BUFFER_SIZE];
static int kb_head = 0;
static int kb_tail = 0;

// Push key event
static void kb_push_event(keyboard_event_t* event) {
    int next = (kb_head + 1) % KB_BUFFER_SIZE;
    if (next != kb_tail) {
        kb_buffer[kb_head] = *event;
        kb_head = next;
    }
}

// Pop key event
int kb_pop_event(keyboard_event_t* event) {
    if (kb_head == kb_tail) return 0;
    *event = kb_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUFFER_SIZE;
    return 1;
}

// Keyboard interrupt handler
static void keyboard_irq_handler(void) {
    uint8_t scancode = inb(KB_DATA_PORT);
    
    // Key release
    if (scancode & 0x80) {
        uint8_t released = scancode & 0x7F;
        
        if (released == 0x2A || released == 0x36) shift_pressed = 0;
        if (released == 0x1D) ctrl_pressed = 0;
        if (released == 0x38) alt_pressed = 0;
        
        return;
    }
    
    // Special keys
    if (scancode == 0x2A || scancode == 0x36) { shift_pressed = 1; return; }
    if (scancode == 0x1D) { ctrl_pressed = 1; return; }
    if (scancode == 0x38) { alt_pressed = 1; return; }
    if (scancode == 0x3A) { caps_lock = !caps_lock; return; }
    
    // Map scancode to character
    char c = 0;
    if (shift_pressed) {
        c = keymap_shift[scancode];
    } else {
        c = keymap[scancode];
    }
    
    // Caps lock for letters
    if (caps_lock && c >= 'a' && c <= 'z') c -= 32;
    else if (caps_lock && c >= 'A' && c <= 'Z') c += 32;
    
    // Create and push event
    keyboard_event_t event;
    event.key = scancode;
    event.pressed = 1;
    event.ctrl = ctrl_pressed;
    event.alt = alt_pressed;
    event.shift = shift_pressed;
    
    kb_push_event(&event);
}

// Initialize keyboard
void keyboard_init(void) {
    // Register IRQ1 handler
    irq_register(1, keyboard_irq_handler);
    
    // Unmask keyboard IRQ
    irq_unmask(1);
}
