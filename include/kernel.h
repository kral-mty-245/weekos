#ifndef WEEKOS_KERNEL_H
#define WEEKOS_KERNEL_H

#include "types.h"
#include <stdarg.h>

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

enum vga_color { VGA_BLACK=0, VGA_BLUE=1, VGA_GREEN=2, VGA_CYAN=3, VGA_RED=4, VGA_MAGENTA=5, VGA_BROWN=6, VGA_LIGHT_GREY=7, VGA_DARK_GREY=8, VGA_LIGHT_BLUE=9, VGA_LIGHT_GREEN=10, VGA_LIGHT_CYAN=11, VGA_LIGHT_RED=12, VGA_LIGHT_MAGENTA=13, VGA_YELLOW=14, VGA_WHITE=15 };

typedef void (*entry_func_t)(void);
typedef struct { uint32_t* buffer; uint32_t width; uint32_t height; uint32_t pitch; } framebuffer_t;
typedef struct { char name[256]; uint32_t size; uint32_t type; uint32_t inode; } vfs_node_t;
typedef struct { uint32_t key; uint8_t pressed; uint8_t ctrl; uint8_t alt; uint8_t shift; } keyboard_event_t;
typedef struct { int32_t x; int32_t y; uint8_t left_button; uint8_t right_button; uint8_t middle_button; } mouse_event_t;

void outb(uint16_t p, uint8_t v); uint8_t inb(uint16_t p); void outw(uint16_t p, uint16_t v); uint16_t inw(uint16_t p);
size_t strlen(const char* s); void strcpy(char* d, const char* s); void strcat(char* d, const char* s); int strcmp(const char* a, const char* b);
char* strchr(const char* s, int c); char* strrchr(const char* s, int c); char* strtok(char* s, const char* d);
void* memset(void* s, int c, size_t n); void* memcpy(void* d, const void* s, size_t n); int memcmp(const void* a, const void* b, size_t n);
char* itoa(int v, char* s, int b); char* utoa(unsigned int v, char* s, int b); int sprintf(char* s, const char* f, ...); int kprintf(const char* f, ...);

void vga_clear(void); void vga_putchar(char c, uint8_t color); void vga_puts(const char* s, uint8_t color);
void vga_puts_at(const char* s, int x, int y, uint8_t c); void vga_put_char_at(char c, int x, int y, uint8_t color);
void vga_disable_cursor(void); void vga_enable_cursor(uint8_t s, uint8_t e);
void vga_clear_text(void); void vga_putchar_text(char c, uint8_t color); void vga_puts_text(const char* s, uint8_t color);

void kernel_main(void); void boot_screen_show(void); void boot_screen_progress(int p);

void* kmalloc(size_t s); void* kcalloc(size_t n, size_t s); void kfree(void* p); void heap_init(void);
pid_t process_create(const char* n, entry_func_t e, uint32_t p); void process_exit(pid_t p);
void scheduler_init(void); void scheduler_start(void); void scheduler_tick(void); void sleep(uint32_t ms);
void timer_init(uint32_t f); uint32_t timer_get_ticks(void);
void irq_init(void); void irq_handler(registers_t* r); void irq_unmask(uint8_t i); void irq_mask(uint8_t i);
void irq_eoi(uint8_t i); void irq_register(uint8_t i, void (*h)(void));
void keyboard_init(void); void mouse_init(void);
int vfs_init(void); int vfs_mkdir(const char* p); int vfs_create(const char* p, uint32_t m); int vfs_open(const char* p, int f);
int disk_init(void); int disk_read_sectors(uint32_t l, uint8_t c, void* b); int disk_write_sectors(uint32_t l, uint8_t c, const void* b);
void io_wait(void); uint16_t port_word_in(uint16_t p); void port_word_out(uint16_t p, uint16_t d);

void fb_init(framebuffer_t* fb); void fb_put_pixel(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t c);
void fb_fill_rect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t c);
void fb_blur_rect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, int r);
uint32_t fb_alpha_blend(uint32_t bg, uint32_t fg, uint8_t a);
void fb_draw_glass_panel(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t o);
void fb_draw_rounded_rect(framebuffer_t* fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t r, uint32_t c);

void desktop_init(void); void theme_set_light(void); void theme_set_dark(void);
void settings_draw(framebuffer_t* fb); void settings_set_category(int c);
void settings_toggle_dark_mode(void); void settings_toggle_defender(void); void settings_toggle_wifi(void);
void settings_toggle_bluetooth(void); void settings_toggle_night_light(void);
void quicksettings_draw(framebuffer_t* fb); void quicksettings_show(void); void quicksettings_hide(void);
void defender_draw(framebuffer_t* fb); void defender_realtime_check(const char* f);
void taskmanager_draw(framebuffer_t* fb); void taskmanager_set_tab(int t);
void explorer_draw(framebuffer_t* fb); void explorer_add_tab(const char* p);
void term_init(bool b); void term_draw(framebuffer_t* fb); void term_handle_input(void* e); void term_puts(const char* s);
void snipping_draw_overlay(framebuffer_t* fb); void snipping_start(framebuffer_t* fb, int m); void snipping_stop(void);
void sticky_init(void); void sticky_draw_all(framebuffer_t* fb);
void store_init(void); void store_draw_main(framebuffer_t* fb);
void store_add_app(const char* n, const char* sd, const char* d, const char* v, int t, int cat, const char* dev, uint32_t sz, uint32_t dl, float r, uint32_t p, bool f, uint32_t pr, uint32_t a, bool w, bool feat, const char* cl);
void installer_draw(framebuffer_t* fb);
void wine_init(void); void wine_draw_settings(framebuffer_t* fb);
void wifi_init(void); void wifi_enable(void); void wifi_disable(void); void wifi_scan_networks(void); bool wifi_is_connected(void);
void bluetooth_init(void); void bluetooth_enable(void); void bluetooth_disable(void); void bluetooth_start_discovery(void); bool bluetooth_is_enabled(void);
void net_init(void);
#endif
