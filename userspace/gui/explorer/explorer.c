// WeeK OS - File Explorer
// Windows 11 style with tabs, quick access

#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// File Explorer State
// ============================================
#define MAX_TABS 10
#define MAX_ITEMS 256
#define MAX_PATH 260

typedef struct {
    char     name[128];
    char     path[MAX_PATH];
    uint32_t size;
    uint32_t type;    // 0=file, 1=folder, 2=drive
    uint32_t icon;    // Icon type
    bool     selected;
} file_item_t;

typedef struct {
    char path[MAX_PATH];
    bool active;
} tab_t;

typedef struct {
    tab_t     tabs[MAX_TABS];
    int       active_tab;
    int       tab_count;
    file_item_t items[MAX_ITEMS];
    int       item_count;
    char      current_path[MAX_PATH];
    bool      show_hidden;
    bool      show_extensions;
    int       view_mode;  // 0=large icons, 1=medium, 2=small, 3=list, 4=details
} explorer_state_t;

static explorer_state_t explorer = {
    .active_tab = 0,
    .tab_count = 1,
    .item_count = 0,
    .show_hidden = false,
    .show_extensions = true,
    .view_mode = 3  // List view
};

// ============================================
// Quick Access Items
// ============================================
typedef struct {
    char name[64];
    char path[MAX_PATH];
    uint32_t icon;
} quick_access_item_t;

static const quick_access_item_t quick_access[] = {
    {"Sik Kullanilan", "/home/user", 0},
    {"Son Dosyalar", "/home/user/recent", 1},
    {"Masaustu", "/home/user/Desktop", 2},
    {"Indirilenler", "/home/user/Downloads", 3},
    {"Belgeler", "/home/user/Documents", 4},
    {"Gorseller", "/home/user/Pictures", 5},
    {"Muzik", "/home/user/Music", 6},
    {"Videolar", "/home/user/Videos", 7},
    {"Bilgisayar", "/", 8},
    {"Ag", "/network", 9},
    {"Geri Donusum Kutusu", "/trash", 10}
};

#define QUICK_ACCESS_COUNT 11

// ============================================
// Navigation Pane
// ============================================
void explorer_draw_nav_pane(framebuffer_t* fb, int x, int y, int width, int height) {
    // Glass background
    fb_draw_glass_panel(fb, x, y, width, height, 0x30);
    
    // Quick Access header
    fb_fill_rect(fb, x + 10, y + 10, width - 20, 24, 0x1A1A1A);
    
    // Quick Access items
    for (int i = 0; i < QUICK_ACCESS_COUNT; i++) {
        int item_y = y + 40 + i * 28;
        
        // Icon
        fb_fill_rect(fb, x + 15, item_y + 4, 16, 16, 0x0078D4);
        
        // Name (placeholder)
        fb_fill_rect(fb, x + 36, item_y + 5, 120, 14, 0x1A1A1A);
        
        // Expand arrow (for folders)
        if (i < 8) {
            fb_fill_rect(fb, x + width - 20, item_y + 6, 8, 8, 0x666666);
        }
    }
}

// ============================================
// Tab Bar
// ============================================
void explorer_draw_tabs(framebuffer_t* fb, int x, int y, int width) {
    int tab_width = 150;
    int tab_x = x;
    
    for (int i = 0; i < explorer.tab_count; i++) {
        uint32_t bg = (i == explorer.active_tab) ? 0x0078D4 : 0xE0E0E0;
        fb_fill_rect(fb, tab_x, y, tab_width, 32, bg);
        
        // Tab title (placeholder)
        fb_fill_rect(fb, tab_x + 10, y + 8, 100, 14, 0x1A1A1A);
        
        // Close button
        fb_fill_rect(fb, tab_x + tab_width - 24, y + 8, 12, 12, 0x999999);
        
        tab_x += tab_width + 4;
    }
    
    // New tab button
    fb_fill_rect(fb, tab_x, y, 32, 32, 0x0078D4);
}

// ============================================
// Toolbar
// ============================================
void explorer_draw_toolbar(framebuffer_t* fb, int x, int y, int width) {
    fb_fill_rect(fb, x, y, width, 40, 0xF0F0F0);
    
    // Back button
    fb_fill_rect(fb, x + 10, y + 8, 24, 24, 0x0078D4);
    
    // Forward button
    fb_fill_rect(fb, x + 40, y + 8, 24, 24, 0xCCCCCC);
    
    // Up button
    fb_fill_rect(fb, x + 70, y + 8, 24, 24, 0x0078D4);
    
    // Address bar
    fb_draw_glass_panel(fb, x + 110, y + 6, width - 250, 28, 0x40);
    fb_fill_rect(fb, x + 120, y + 12, 20, 16, 0x666666);  // Folder icon
    
    // Search bar
    fb_draw_glass_panel(fb, x + width - 130, y + 6, 120, 28, 0x40);
    fb_fill_rect(fb, x + width - 120, y + 12, 16, 16, 0x666666);  // Search icon
    
    // View options
    fb_fill_rect(fb, x + width - 200, y + 8, 24, 24, 0x666666);
    
    // Sort options
    fb_fill_rect(fb, x + width - 170, y + 8, 24, 24, 0x666666);
}

// ============================================
// File List
// ============================================
void explorer_draw_file_list(framebuffer_t* fb, int x, int y, int width, int height) {
    // Column headers (details view)
    if (explorer.view_mode == 4) {
        fb_fill_rect(fb, x, y, width, 28, 0xE0E0E0);
        
        const char* headers[] = {"Ad", "Tarih", "Tur", "Boyut"};
        int col_x = x + 10;
        int col_widths[] = {200, 150, 100, 80};
        
        for (int i = 0; i < 4; i++) {
            fb_fill_rect(fb, col_x, y + 7, col_widths[i], 14, 0x1A1A1A);
            col_x += col_widths[i] + 10;
        }
        y += 30;
    }
    
    // File items
    for (int i = 0; i < explorer.item_count; i++) {
        file_item_t* item = &explorer.items[i];
        
        uint32_t row_bg = item->selected ? 0x0078D4 : 
                         (i % 2 == 0 ? 0xFFFFFF : 0xF8F8F8);
        
        if (explorer.view_mode == 4) {
            // Details view
            fb_fill_rect(fb, x, y, width, 24, row_bg);
            
            // Icon
            fb_fill_rect(fb, x + 10, y + 4, 16, 16, item->type == 1 ? 0xFFD700 : 0x0078D4);
            
            // Name
            fb_fill_rect(fb, x + 30, y + 5, 180, 14, 0x1A1A1A);
            
            // Type
            fb_fill_rect(fb, x + 220, y + 5, 100, 14, 0x666666);
            
            // Size
            fb_fill_rect(fb, x + 340, y + 5, 80, 14, 0x666666);
            
            y += 24;
        } else {
            // Icon view
            fb_fill_rect(fb, x + 10, y, 64, 64, row_bg);
            fb_fill_rect(fb, x + 10, y + 68, 64, 16, 0x1A1A1A);
            y += 90;
        }
    }
}

// ============================================
// Status Bar
// ============================================
void explorer_draw_status_bar(framebuffer_t* fb, int x, int y, int width) {
    fb_fill_rect(fb, x, y, width, 28, 0xF0F0F0);
    
    // Item count
    fb_fill_rect(fb, x + 10, y + 6, 150, 14, 0x666666);
    
    // Selection info
    fb_fill_rect(fb, x + 200, y + 6, 150, 14, 0x666666);
}

// ============================================
// Full Explorer UI
// ============================================
void explorer_draw(framebuffer_t* fb) {
    // Clear screen
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0xF0F0F0);
    
    int nav_width = 200;
    int toolbar_height = 40;
    int tab_height = 32;
    int status_height = 28;
    
    // Title bar
    fb_fill_rect(fb, 0, 0, fb->width, 32, 0x0078D4);
    
    // Tab bar
    explorer_draw_tabs(fb, 0, 32, fb->width);
    
    // Toolbar
    explorer_draw_toolbar(fb, 0, 64, fb->width);
    
    // Navigation pane
    explorer_draw_nav_pane(fb, 0, 104, nav_width, fb->height - 104 - status_height);
    
    // File list
    explorer_draw_file_list(fb, nav_width, 104, 
                           fb->width - nav_width, 
                           fb->height - 104 - status_height);
    
    // Status bar
    explorer_draw_status_bar(fb, 0, fb->height - status_height, fb->width);
}

// ============================================
// Explorer Actions
// ============================================
void explorer_add_tab(const char* path) {
    if (explorer.tab_count >= MAX_TABS) return;
    
    tab_t* tab = &explorer.tabs[explorer.tab_count];
    strcpy(tab->path, path);
    tab->active = true;
    explorer.tab_count++;
}

void explorer_close_tab(int index) {
    if (index < 0 || index >= explorer.tab_count) return;
    
    for (int i = index; i < explorer.tab_count - 1; i++) {
        explorer.tabs[i] = explorer.tabs[i + 1];
    }
    explorer.tab_count--;
    
    if (explorer.active_tab >= explorer.tab_count) {
        explorer.active_tab = explorer.tab_count - 1;
    }
}

void explorer_navigate(const char* path) {
    strcpy(explorer.current_path, path);
    // Load directory contents
    // In real implementation, this would read the filesystem
}

void explorer_refresh(void) {
    // Reload current directory
    explorer_navigate(explorer.current_path);
}
