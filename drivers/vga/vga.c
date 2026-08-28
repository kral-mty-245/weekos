// WeeK OS - VGA Text Mode Driver

#include "../../include/kernel.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t* const VGA = (uint16_t*)VGA_ADDRESS;
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t current_color = 0x0F;

// Set cursor position
static void vga_set_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// Scroll screen
static void vga_scroll(void) {
    if (cursor_y >= VGA_HEIGHT) {
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            VGA[i] = VGA[i + VGA_WIDTH];
        }
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            VGA[i] = (current_color << 8) | ' ';
        }
        cursor_y = VGA_HEIGHT - 1;
    }
}

// VGA putchar
void vga_putchar_text(char c, uint8_t color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA[cursor_y * VGA_WIDTH + cursor_x] = (color << 8) | ' ';
        }
    } else {
        VGA[cursor_y * VGA_WIDTH + cursor_x] = (color << 8) | c;
        cursor_x++;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    vga_scroll();
    vga_set_cursor(cursor_x, cursor_y);
}

// VGA puts
void vga_puts_text(const char* str, uint8_t color) {
    while (*str) {
        vga_putchar_text(*str++, color);
    }
}

// VGA clear
void vga_clear_text(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA[i] = (current_color << 8) | ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
    vga_set_cursor(0, 0);
}
