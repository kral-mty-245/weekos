// WeeK OS - Desktop Environment
// Liquid Glass Desktop with Windows 11 style

#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// Window Manager
// ============================================
#define MAX_WINDOWS 64
#define WIN_TITLE_HEIGHT 32
#define WIN_MIN_WIDTH 200
#define WIN_MIN_HEIGHT 100

typedef struct {
    uint32_t id;
    char     title[128];
    int32_t  x, y;
    uint32_t w, h;
    bool     visible;
    bool     maximized;
    bool     minimized;
    bool     focused;
    uint32_t* content_buffer;
    uint32_t bg_color;
    uint32_t title_color;
    uint32_t border_color;
} window_t;

static window_t windows[MAX_WINDOWS];
static uint32_t window_count = 0;
static uint32_t active_window = 0;
static framebuffer_t desktop_fb;

// ============================================
// Desktop Theme
// ============================================
typedef struct {
    uint32_t wallpaper_primary;
    uint32_t wallpaper_secondary;
    uint32_t taskbar_bg;
    uint32_t taskbar_text;
    uint32_t window_bg;
    uint32_t window_title;
    uint32_t window_border;
    uint32_t accent_color;
    uint32_t text_color;
    uint32_t glass_color;
    uint8_t  glass_opacity;
} theme_t;

// Default Light Theme (White & Blue)
static theme_t light_theme = {
    .wallpaper_primary = 0x1A3A5C,    // Dark blue
    .wallpaper_secondary = 0x2D5F8A,  // Medium blue
    .taskbar_bg = 0xF0F0F0,          // Light grey
    .taskbar_text = 0x1A1A1A,        // Dark text
    .window_bg = 0xFFFFFF,           // White
    .window_title = 0x0078D4,        // Microsoft Blue
    .window_border = 0x0078D4,       // Blue border
    .accent_color = 0x0078D4,        // Blue accent
    .text_color = 0x1A1A1A,          // Dark text
    .glass_color = 0xE8F4FD,         // Light blue glass
    .glass_opacity = 0x40,           // Glass opacity
};

// Dark Theme
static theme_t dark_theme = {
    .wallpaper_primary = 0x0A1628,    // Very dark blue
    .wallpaper_secondary = 0x1A2A4A,  // Dark blue
    .taskbar_bg = 0x202020,          // Dark grey
    .taskbar_text = 0xFFFFFF,        // White text
    .window_bg = 0x2D2D2D,          // Dark background
    .window_title = 0x0078D4,        // Blue title
    .window_border = 0x3D3D3D,       // Dark border
    .accent_color = 0x0078D4,        // Blue accent
    .text_color = 0xFFFFFF,          // White text
    .glass_color = 0x1A1A2E,         // Dark glass
    .glass_opacity = 0x60,           // More opaque
};

static theme_t* current_theme = &light_theme;

// ============================================
// Taskbar
// ============================================
#define TASKBAR_HEIGHT 48
#define TASKBAR_BUTTON_SIZE 40
#define SYSTEM_TRAY_WIDTH 200

static bool taskbar_visible = true;

// Draw taskbar
void draw_taskbar(framebuffer_t* fb) {
    int y = fb->height - TASKBAR_HEIGHT;
    
    // Taskbar background (liquid glass)
    fb_draw_glass_panel(fb, 0, y, fb->width, TASKBAR_HEIGHT, 0x80);
    
    // Taskbar buttons (centered)
    int btn_x = fb->width / 2 - 100;
    
    // Start button
    fb_fill_rect(fb, btn_x, y + 4, 32, 32, current_theme->accent_color);
    // Windows icon placeholder
    for (int i = 0; i < 3; i++) {
        fb_fill_rect(fb, btn_x + 4 + i * 10, y + 12, 6, 6, 0xFFFFFF);
        fb_fill_rect(fb, btn_x + 4 + i * 10, y + 22, 6, 6, 0xFFFFFF);
    }
    
    // Search button
    btn_x += 48;
    fb_fill_rect(fb, btn_x, y + 4, 200, 32, 0x0078D4);
    
    // Task View button
    btn_x += 216;
    fb_fill_rect(fb, btn_x, y + 4, 32, 32, 0x0078D4);
    
    // Pinned apps
    btn_x += 48;
    for (int i = 0; i < 5; i++) {
        fb_fill_rect(fb, btn_x + i * 48, y + 4, 32, 32, 0x0078D4);
    }
    
    // System tray (right side)
    int tray_x = fb->width - SYSTEM_TRAY_WIDTH;
    fb_fill_rect(fb, tray_x, y, SYSTEM_TRAY_WIDTH, TASKBAR_HEIGHT, current_theme->taskbar_bg);
    
    // Clock
    // (In real kernel, we'd read RTC)
    const char* time = "15:43";
    // vga_puts_at(time, tray_x + 10, y + 2, current_theme->taskbar_text);
    
    // Notification icon
    fb_fill_rect(fb, tray_x + 140, y + 8, 24, 24, current_theme->accent_color);
    
    // Battery
    fb_fill_rect(fb, tray_x + 170, y + 12, 20, 16, current_theme->text_color);
    
    // WiFi
    fb_fill_rect(fb, tray_x + 120, y + 12, 20, 16, current_theme->accent_color);
    
    // Volume
    fb_fill_rect(fb, tray_x + 100, y + 12, 16, 16, current_theme->text_color);
}

// ============================================
// Window Manager Functions
// ============================================

// Create a new window
uint32_t wm_create_window(const char* title, int32_t x, int32_t y, uint32_t w, uint32_t h) {
    if (window_count >= MAX_WINDOWS) return 0;
    
    window_t* win = &windows[window_count];
    win->id = window_count + 1;
    strcpy(win->title, title);
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
    win->visible = true;
    win->maximized = false;
    win->minimized = false;
    win->focused = false;
    win->bg_color = current_theme->window_bg;
    win->title_color = current_theme->window_title;
    win->border_color = current_theme->window_border;
    
    // Allocate content buffer
    win->content_buffer = (uint32_t*)kmalloc(w * h * 4);
    
    window_count++;
    return win->id;
}

// Close a window
void wm_close_window(uint32_t id) {
    for (uint32_t i = 0; i < window_count; i++) {
        if (windows[i].id == id) {
            if (windows[i].content_buffer) {
                kfree(windows[i].content_buffer);
            }
            windows[i].visible = false;
            break;
        }
    }
}

// Draw window with liquid glass effect
void wm_draw_window(framebuffer_t* fb, uint32_t id) {
    window_t* win = NULL;
    for (uint32_t i = 0; i < window_count; i++) {
        if (windows[i].id == id) {
            win = &windows[i];
            break;
        }
    }
    
    if (!win || !win->visible || win->minimized) return;
    
    // Window shadow
    fb_fill_rect(fb, win->x + 4, win->y + 4, win->w, win->h, 0x000000);
    
    // Window body (liquid glass)
    fb_draw_glass_panel(fb, win->x, win->y, win->w, win->h, current_theme->glass_opacity);
    
    // Title bar
    fb_fill_rect(fb, win->x, win->y, win->w, WIN_TITLE_HEIGHT, win->title_color);
    
    // Title text (simplified)
    for (int i = 0; win->title[i] && i < 30; i++) {
        // Each character is approx 8 pixels wide
        fb_fill_rect(fb, win->x + 12 + i * 8, win->y + 10, 6, 14, 0xFFFFFF);
    }
    
    // Window control buttons
    // Close button (X)
    fb_fill_rect(fb, win->x + win->w - 46, win->y + 6, 34, 20, 0xE81123);
    // Minimize button
    fb_fill_rect(fb, win->x + win->w - 82, win->y + 6, 34, 20, 0x0078D4);
    // Maximize button
    fb_fill_rect(fb, win->x + win->w - 118, win->y + 6, 34, 20, 0x0078D4);
}

// ============================================
// Desktop Background
// ============================================
void draw_wallpaper(framebuffer_t* fb) {
    // Draw gradient wallpaper
    for (uint32_t y = 0; y < fb->height; y++) {
        for (uint32_t x = 0; x < fb->width; x++) {
            uint32_t r, g, b;
            
            // Create gradient from primary to secondary color
            float factor = (float)y / fb->height;
            
            r = (uint32_t)((current_theme->wallpaper_primary & 0xFF) * (1 - factor) + 
                           (current_theme->wallpaper_secondary & 0xFF) * factor);
            g = (uint32_t)(((current_theme->wallpaper_primary >> 8) & 0xFF) * (1 - factor) + 
                           ((current_theme->wallpaper_secondary >> 8) & 0xFF) * factor);
            b = (uint32_t)(((current_theme->wallpaper_primary >> 16) & 0xFF) * (1 - factor) + 
                           ((current_theme->wallpaper_secondary >> 16) & 0xFF) * factor);
            
            // Add subtle wave pattern
            float wave = 0.1f * (float)y / 50.0f;
            uint32_t wave_mod = (uint32_t)(wave * 20) % 40;
            
            r += wave_mod;
            g += wave_mod;
            b += wave_mod;
            
            fb_put_pixel(fb, x, y, (b << 16) | (g << 8) | r);
        }
    }
}

// ============================================
// Desktop Icon
// ============================================
typedef struct {
    char name[64];
    int32_t x, y;
    uint32_t icon_color;
    bool visible;
} desktop_icon_t;

#define MAX_ICONS 32
static desktop_icon_t desktop_icons[MAX_ICONS];
static int icon_count = 0;

void add_desktop_icon(const char* name, int32_t x, int32_t y, uint32_t color) {
    if (icon_count >= MAX_ICONS) return;
    
    desktop_icon_t* icon = &desktop_icons[icon_count];
    strcpy(icon->name, name);
    icon->x = x;
    icon->y = y;
    icon->icon_color = color;
    icon->visible = true;
    icon_count++;
}

void draw_desktop_icons(framebuffer_t* fb) {
    for (int i = 0; i < icon_count; i++) {
        desktop_icon_t* icon = &desktop_icons[i];
        if (!icon->visible) continue;
        
        // Icon background
        fb_fill_rect(fb, icon->x, icon->y, 48, 48, icon->icon_color);
        
        // Icon label (simplified - just a rectangle)
        fb_fill_rect(fb, icon->x - 4, icon->y + 52, 56, 16, 0x000000);
    }
}

// ============================================
// Desktop Context Menu
// ============================================
void show_context_menu(framebuffer_t* fb, int32_t x, int32_t y) {
    // Glass context menu
    fb_draw_glass_panel(fb, x, y, 200, 180, 0x90);
    
    // Menu items
    const char* items[] = {
        "Yenile",
        "Yeni Klasor",
        "Yeni Dosya",
        "---",
        "Yapi Ayarlari",
        "Gorunum",
        "---",
        "Hosgeldin WeeK OS"
    };
    
    for (int i = 0; i < 8; i++) {
        if (items[i][0] == '-') {
            // Separator
            fb_fill_rect(fb, x + 10, y + 10 + i * 22, 180, 1, 0xCCCCCC);
        } else {
            // Menu item
            fb_fill_rect(fb, x + 10, y + 10 + i * 22, 180, 20, 0x000000);
        }
    }
}

// ============================================
// Desktop Main Function
// ============================================
void desktop_init(void) {
    // Initialize framebuffer
    fb_init(&desktop_fb);
    
    // Set light theme as default
    current_theme = &light_theme;
    
    // Draw wallpaper
    draw_wallpaper(&desktop_fb);
    
    // Add default desktop icons
    add_desktop_icon("Bilgisayar", 50, 50, 0x0078D4);
    add_desktop_icon("Gercek Klasor", 50, 140, 0x0078D4);
    add_desktop_icon("Geri Donusum Kutusu", 50, 230, 0x0078D4);
    add_desktop_icon("WeeK Store", 50, 320, 0x0078D4);
    add_desktop_icon("Dosya Gezgini", 50, 410, 0x0078D4);
    
    // Draw desktop icons
    draw_desktop_icons(&desktop_fb);
    
    // Draw taskbar
    draw_taskbar(&desktop_fb);
    
    // Create default windows
    wm_create_window("Dosya Gezgini", 100, 100, 800, 500);
    wm_create_window("Ayarlar", 200, 150, 900, 600);
    
    // Draw windows
    for (uint32_t i = 0; i < window_count; i++) {
        wm_draw_window(&desktop_fb, windows[i].id);
    }
}

// ============================================
// Theme Switcher
// ============================================
void theme_set_light(void) {
    current_theme = &light_theme;
    draw_wallpaper(&desktop_fb);
    draw_taskbar(&desktop_fb);
}

void theme_set_dark(void) {
    current_theme = &dark_theme;
    draw_wallpaper(&desktop_fb);
    draw_taskbar(&desktop_fb);
}

theme_t* theme_get_current(void) {
    return current_theme;
}
