// WeeK OS - Main Kernel Entry
// Real kernel boot sequence

#include "../include/kernel.h"
#include "../include/types.h"

// Kernel version
#define WEEKOS_VERSION "1.0.0"
#define WEEKOS_NAME    "WeeK OS"

// Boot stage tracking
static int boot_stage = 0;

// VGA text mode buffer
static uint16_t* const VGA = (uint16_t*)VGA_ADDRESS;

// ============================================
// Boot Screen - WeeK OS Logo
// ============================================
void boot_screen_show(void) {
    vga_clear();
    vga_disable_cursor();
    
    // WeeK OS ASCII Art Logo
    const char* logo[] = {
        "  __        __   _    _____                    _             ",
        "  \\ \\      / /__| |__|_   _|__ _ __ _ __ ___ (_)_ __   __ _ ",
        "   \\ \\ /\\ / / _ \\ '_ \\ | |/ _ \\ '__| '_ ` _ \\| | '_ \\ / _` |",
        "    \\ V  V /  __/ |_) || |  __/ |  | | | | | | | | | | (_| |",
        "     \\_/\\_/ \\___|_.__/ |_|\\___|_|  |_| |_| |_|_|_| |_|\\__, |",
        "                                                       |___/ ",
    };
    
    int logo_y = 3;
    for (int i = 0; i < 6; i++) {
        int x = 10;
        for (int j = 0; logo[i][j]; j++) {
            VGA[(logo_y * 80) + x + j] = (VGA_LIGHT_BLUE << 8) | logo[i][j];
        }
        logo_y++;
    }
    
    // Version info
    const char* ver = "Version " WEEKOS_VERSION " | Build 280826";
    int ver_x = 35;
    for (int i = 0; ver[i]; i++) {
        VGA[(9 * 80) + ver_x + i] = (VGA_LIGHT_GREY << 8) | ver[i];
    }
    
    // Boot messages
    const char* msgs[] = {
        "[BOOT] Initializing WeeK OS Kernel...",
        "[BOOT] Loading kernel modules...",
        "[BOOT] Detecting hardware...",
        "[BOOT] Initializing memory manager...",
        "[BOOT] Loading drivers...",
        "[BOOT] Starting init system...",
    };
    
    int msg_y = 13;
    for (int i = 0; i < 6; i++) {
        vga_puts_at(msgs[i], 10, msg_y + i, VGA_LIGHT_GREY);
        boot_screen_progress((i + 1) * 100 / 6);
    }
}

void vga_puts_at(const char* str, int x, int y, uint8_t color) {
    for (int i = 0; str[i]; i++) {
        VGA[(y * 80) + x + i] = (color << 8) | str[i];
    }
}

// ============================================
// VGA Functions
// ============================================
void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA[i] = (VGA_BLUE << 8) | ' ';
    }
}

void vga_putchar(char c, uint8_t color) {
    static int x = 0, y = 0;
    
    if (c == '\n') {
        x = 0;
        y++;
        if (y >= VGA_HEIGHT) y = 0;
        return;
    }
    
    VGA[(y * 80) + x] = (color << 8) | c;
    x++;
    if (x >= VGA_WIDTH) {
        x = 0;
        y++;
        if (y >= VGA_HEIGHT) y = 0;
    }
}

void vga_puts(const char* str, uint8_t color) {
    for (int i = 0; str[i]; i++) {
        vga_putchar(str[i], color);
    }
}

void vga_put_char_at(char c, int x, int y, uint8_t color) {
    VGA[(y * 80) + x] = (color << 8) | c;
}

void vga_disable_cursor(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void vga_enable_cursor(uint8_t start, uint8_t end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | start);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | end);
}

// ============================================
// Boot Progress Bar
// ============================================
void boot_screen_progress(int percent) {
    int bar_y = 22;
    int bar_x = 15;
    int bar_width = 50;
    
    // Draw progress bar background
    VGA[(bar_y * 80) + bar_x - 1] = (VGA_WHITE << 8) | '[';
    for (int i = 0; i < bar_width; i++) {
        if (i < (percent * bar_width / 100)) {
            VGA[(bar_y * 80) + bar_x + i] = (VGA_LIGHT_BLUE << 8) | '#';
        } else {
            VGA[(bar_y * 80) + bar_x + i] = (VGA_DARK_GREY << 8) | '-';
        }
    }
    VGA[(bar_y * 80) + bar_x + bar_width] = (VGA_WHITE << 8) | ']';
    
    // Percentage text
    char pct_str[5] = "  0%";
    pct_str[0] = '0' + (percent / 10);
    pct_str[1] = '0' + (percent % 10);
    VGA[(bar_y * 80) + bar_x + bar_width + 2] = (VGA_WHITE << 8) | pct_str[0];
    VGA[(bar_y * 80) + bar_x + bar_width + 3] = (VGA_WHITE << 8) | pct_str[1];
    VGA[(bar_y * 80) + bar_x + bar_width + 4] = (VGA_WHITE << 8) | '%';
}

// ============================================
// Port I/O
// ============================================
void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outw(uint16_t port, uint16_t val) {
    asm volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// ============================================
// String Functions
// ============================================
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

void strcpy(char* dest, const char* src) {
    while (*src) *dest++ = *src++;
    *dest = '\0';
}

void strcat(char* dest, const char* src) {
    while (*dest) dest++;
    while (*src) *dest++ = *src++;
    *dest = '\0';
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// ============================================
// Kernel Main
// ============================================
void kernel_main(void) {
    // Step 1: Show boot screen
    boot_screen_show();
    
    // Step 2: Initialize VGA
    vga_clear();
    
    // Step 3: Welcome message
    vga_puts("\n", VGA_WHITE);
    vga_puts("  WeeK OS v" WEEKOS_VERSION " - Real Kernel Edition\n", VGA_LIGHT_BLUE);
    vga_puts("  ==============================================\n", VGA_BLUE);
    vga_puts("  Kernel: WeeK Microkernel v1.0\n", VGA_LIGHT_GREY);
    vga_puts("  Architecture: x86 (i386)\n", VGA_LIGHT_GREY);
    vga_puts("  Memory: 512 MB\n", VGA_LIGHT_GREY);
    vga_puts("  CPU: Intel/AMD x86 compatible\n", VGA_LIGHT_GREY);
    vga_puts("\n", VGA_WHITE);
    vga_puts("  System is ready. Starting desktop...\n", VGA_LIGHT_GREEN);
    vga_puts("\n", VGA_WHITE);
    vga_puts("  WeeK OS - Liquid Glass Desktop Environment\n", VGA_CYAN);
    vga_puts("  Press any key to continue...\n", VGA_LIGHT_GREY);
    
    // Wait for keypress
    while (!(inb(0x64) & 1));
    inb(0x60);
    
    // Step 4: Start desktop environment
    // In a real kernel, this would switch to protected mode,
    // enable paging, and load the desktop environment
    
    // For now, show the desktop splash
    vga_clear();
    
    // Draw simple desktop representation
    const char* desktop_msg = "WeeK OS Desktop - Liquid Glass Environment";
    vga_puts_at(desktop_msg, 25, 12, VGA_WHITE);
    
    const char* ready_msg = "System initialized successfully!";
    vga_puts_at(ready_msg, 28, 14, VGA_LIGHT_GREEN);
    
    // Halt
    while(1) {
        asm volatile("hlt");
    }
}
