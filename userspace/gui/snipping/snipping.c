// WeeK OS - Snipping Tool
// Win+Shift+S screen capture

#define MAX_PATH 260
#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// Snipping Tool State
// ============================================
typedef enum {
    SNIP_NONE = 0,
    SNIP_RECTANGLE,
    SNIP_FREEFORM,
    SNIP_WINDOW,
    SNIP_FULLSCREEN
} snip_mode_t;

typedef struct {
    snip_mode_t mode;
    bool     active;
    int32_t  start_x, start_y;
    int32_t  end_x, end_y;
    bool     selecting;
    uint32_t* screenshot;
    uint32_t  screenshot_width;
    uint32_t  screenshot_height;
    bool     saved;
    char     save_path[MAX_PATH];
} snipping_state_t;

static snipping_state_t snipping = {
    .mode = SNIP_NONE,
    .active = false,
    .selecting = false,
    .screenshot = NULL,
    .saved = false
};

// ============================================
// Take Screenshot
// ============================================
void snipping_take_screenshot(framebuffer_t* fb) {
    // Copy framebuffer to screenshot buffer
    uint32_t size = fb->width * fb->height * 4;
    snipping.screenshot = (uint32_t*)kmalloc(size);
    
    if (snipping.screenshot) {
        uint32_t* src = fb->buffer;
        uint32_t* dst = snipping.screenshot;
        
        for (uint32_t i = 0; i < fb->width * fb->height; i++) {
            dst[i] = src[i];
        }
        
        snipping.screenshot_width = fb->width;
        snipping.screenshot_height = fb->height;
    }
}

// ============================================
// Snipping Tool UI
// ============================================
void snipping_draw_overlay(framebuffer_t* fb) {
    if (!snipping.active) return;
    
    // Dim the screen
    for (uint32_t i = 0; i < fb->width * fb->height; i++) {
        uint32_t pixel = fb->buffer[i];
        uint8_t r = ((pixel >> 16) & 0xFF) * 0.6f;
        uint8_t g = ((pixel >> 8) & 0xFF) * 0.6f;
        uint8_t b = (pixel & 0xFF) * 0.6f;
        fb->buffer[i] = (r << 16) | (g << 8) | b;
    }
    
    // Draw selection rectangle
    if (snipping.selecting) {
        int32_t x1 = MIN(snipping.start_x, snipping.end_x);
        int32_t y1 = MIN(snipping.start_y, snipping.end_y);
        int32_t x2 = MAX(snipping.start_x, snipping.end_x);
        int32_t y2 = MAX(snipping.start_y, snipping.end_y);
        
        int32_t w = x2 - x1;
        int32_t h = y2 - y1;
        
        // Selection border
        fb_fill_rect(fb, x1, y1, w, 2, 0x0078D4);  // Top
        fb_fill_rect(fb, x1, y2 - 2, w, 2, 0x0078D4);  // Bottom
        fb_fill_rect(fb, x1, y1, 2, h, 0x0078D4);  // Left
        fb_fill_rect(fb, x2 - 2, y1, 2, h, 0x0078D4);  // Right
        
        // Dimensions display
        char dim_str[32];
        // snprintf(dim_str, 32, "%dx%d", w, h);
        fb_fill_rect(fb, x1, y1 - 24, 80, 20, 0x0078D4);
    }
    
    // Toolbar at top
    int toolbar_w = 400;
    int toolbar_h = 48;
    int toolbar_x = (fb->width - toolbar_w) / 2;
    int toolbar_y = 20;
    
    fb_draw_glass_panel(fb, toolbar_x, toolbar_y, toolbar_w, toolbar_h, 0x90);
    
    // Mode buttons
    const char* modes[] = {"Dikdortgen", "Serbest", "Pencere", "Tam Ekran"};
    for (int i = 0; i < 4; i++) {
        int btn_x = toolbar_x + 20 + i * 90;
        uint32_t bg = (snipping.mode == i + 1) ? 0x0078D4 : 0x404040;
        fb_fill_rect(fb, btn_x, toolbar_y + 8, 80, 32, bg);
    }
    
    // Cancel button
    fb_fill_rect(fb, toolbar_x + toolbar_w - 60, toolbar_y + 8, 40, 32, 0xFF0000);
}

// ============================================
// Snipping Tool Actions
// ============================================
void snipping_start(framebuffer_t* fb, snip_mode_t mode) {
    snipping.mode = mode;
    snipping.active = true;
    snipping.selecting = false;
    
    // Take screenshot
    snipping_take_screenshot(fb);
}

void snipping_stop(void) {
    snipping.active = false;
    snipping.selecting = false;
    
    if (snipping.screenshot) {
        kfree(snipping.screenshot);
        snipping.screenshot = NULL;
    }
}

void snipping_start_selection(int32_t x, int32_t y) {
    snipping.selecting = true;
    snipping.start_x = x;
    snipping.start_y = y;
    snipping.end_x = x;
    snipping.end_y = y;
}

void snipping_update_selection(int32_t x, int32_t y) {
    if (snipping.selecting) {
        snipping.end_x = x;
        snipping.end_y = y;
    }
}

void snipping_end_selection(void) {
    snipping.selecting = false;
    
    // Copy selected region
    int32_t x1 = MIN(snipping.start_x, snipping.end_x);
    int32_t y1 = MIN(snipping.start_y, snipping.end_y);
    int32_t x2 = MAX(snipping.start_x, snipping.end_x);
    int32_t y2 = MAX(snipping.start_y, snipping.end_y);
    
    int32_t w = x2 - x1;
    int32_t h = y2 - y1;
    
    if (w > 0 && h > 0) {
        // Save selection
        uint32_t* selection = (uint32_t*)kmalloc(w * h * 4);
        if (selection && snipping.screenshot) {
            for (int32_t y = 0; y < h; y++) {
                for (int32_t x = 0; x < w; x++) {
                    uint32_t src_x = x1 + x;
                    uint32_t src_y = y1 + y;
                    if (src_x < snipping.screenshot_width && src_y < snipping.screenshot_height) {
                        selection[y * w + x] = snipping.screenshot[src_y * snipping.screenshot_width + src_x];
                    }
                }
            }
            
            // Copy to clipboard
            // clipboard_set(selection, w * h * 4);
            
            kfree(selection);
            snipping.saved = true;
        }
    }
    
    snipping_stop();
}

void snipping_copy_to_clipboard(void) {
    // Copy current selection to clipboard
    snipping.saved = true;
}

void snipping_save_to_file(const char* path) {
    strcpy(snipping.save_path, path);
    snipping.saved = true;
}

snipping_state_t* snipping_get_state(void) {
    return &snipping;
}
