// WeeK OS - VGA Framebuffer Driver
// For liquid glass effects and modern desktop

#include "../../include/kernel.h"

#define FB_ADDRESS  0xFD000000  // Linear framebuffer address
#define FB_DEFAULT_W 1920
#define FB_DEFAULT_H 1080
#define FB_DEFAULT_BPP 32

// Framebuffer info
static framebuffer_t fb;
static uint8_t fb_initialized = 0;

// Initialize framebuffer
void fb_init(framebuffer_t* fb_ptr) {
    fb_ptr->buffer = (uint32_t*)FB_ADDRESS;
    fb_ptr->width = FB_DEFAULT_W;
    fb_ptr->height = FB_DEFAULT_H;
    fb_ptr->pitch = fb_ptr->width * 4;  // 4 bytes per pixel (32bpp)
    fb = *fb_ptr;
    fb_initialized = 1;
}

// Put pixel
void fb_put_pixel(framebuffer_t* fb_ptr, uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb_ptr->width || y >= fb_ptr->height) return;
    fb_ptr->buffer[y * (fb_ptr->pitch / 4) + x] = color;
}

// Get pixel
static uint32_t fb_get_pixel(framebuffer_t* fb_ptr, uint32_t x, uint32_t y) {
    if (x >= fb_ptr->width || y >= fb_ptr->height) return 0;
    return fb_ptr->buffer[y * (fb_ptr->pitch / 4) + x];
}

// Fill rectangle
void fb_fill_rect(framebuffer_t* fb_ptr, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    for (uint32_t py = y; py < y + h && py < fb_ptr->height; py++) {
        for (uint32_t px = x; px < x + w && px < fb_ptr->width; px++) {
            fb_ptr->buffer[py * (fb_ptr->pitch / 4) + px] = color;
        }
    }
}

// Alpha blend two colors
uint32_t fb_alpha_blend(uint32_t bg, uint32_t fg, uint8_t alpha) {
    uint8_t bg_r = (bg >> 16) & 0xFF;
    uint8_t bg_g = (bg >> 8) & 0xFF;
    uint8_t bg_b = bg & 0xFF;
    
    uint8_t fg_r = (fg >> 16) & 0xFF;
    uint8_t fg_g = (fg >> 8) & 0xFF;
    uint8_t fg_b = fg & 0xFF;
    
    uint8_t out_r = (alpha * fg_r + (255 - alpha) * bg_r) / 255;
    uint8_t out_g = (alpha * fg_g + (255 - alpha) * bg_g) / 255;
    uint8_t out_b = (alpha * fg_b + (255 - alpha) * bg_b) / 255;
    
    return (out_r << 16) | (out_g << 8) | out_b;
}

// Box blur effect (for liquid glass)
void fb_blur_rect(framebuffer_t* fb_ptr, uint32_t x, uint32_t y, uint32_t w, uint32_t h, int radius) {
    // Simple box blur
    for (uint32_t py = y + radius; py < y + h - radius; py++) {
        for (uint32_t px = x + radius; px < x + w - radius; px++) {
            uint32_t r = 0, g = 0, b = 0;
            int count = 0;
            
            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    uint32_t pixel = fb_get_pixel(fb_ptr, px + dx, py + dy);
                    r += (pixel >> 16) & 0xFF;
                    g += (pixel >> 8) & 0xFF;
                    b += pixel & 0xFF;
                    count++;
                }
            }
            
            r /= count;
            g /= count;
            b /= count;
            
            fb_put_pixel(fb_ptr, px, py, (r << 16) | (g << 8) | b);
        }
    }
}

// Draw glass panel (liquid glass effect)
void fb_draw_glass_panel(framebuffer_t* fb_ptr, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t opacity) {
    // First, blur the background
    fb_blur_rect(fb_ptr, x, y, w, h, 8);
    
    // Then overlay with semi-transparent white/blue
    for (uint32_t py = y; py < y + h && py < fb_ptr->height; py++) {
        for (uint32_t px = x; px < x + w && px < fb_ptr->width; px++) {
            uint32_t bg = fb_get_pixel(fb_ptr, px, py);
            
            // Glass color: light blue with transparency
            uint32_t glass_color = 0xE8F4FD;  // Light blue glass
            uint32_t blended = fb_alpha_blend(bg, glass_color, opacity);
            
            fb_put_pixel(fb_ptr, px, py, blended);
        }
    }
    
    // Add border
    uint32_t border_color = 0x80C8FF;  // Light blue border
    uint32_t border_alpha = 0x80;
    
    // Top border
    for (uint32_t px = x; px < x + w; px++) {
        uint32_t bg = fb_get_pixel(fb_ptr, px, y);
        fb_put_pixel(fb_ptr, px, y, fb_alpha_blend(bg, border_color, border_alpha));
    }
    
    // Bottom border
    for (uint32_t px = x; px < x + w; px++) {
        uint32_t bg = fb_get_pixel(fb_ptr, px, y + h - 1);
        fb_put_pixel(fb_ptr, px, y + h - 1, fb_alpha_blend(bg, border_color, border_alpha));
    }
    
    // Left border
    for (uint32_t py = y; py < y + h; py++) {
        uint32_t bg = fb_get_pixel(fb_ptr, x, py);
        fb_put_pixel(fb_ptr, x, py, fb_alpha_blend(bg, border_color, border_alpha));
    }
    
    // Right border
    for (uint32_t py = y; py < y + h; py++) {
        uint32_t bg = fb_get_pixel(fb_ptr, x + w - 1, py);
        fb_put_pixel(fb_ptr, x + w - 1, py, fb_alpha_blend(bg, border_color, border_alpha));
    }
}

// Draw rounded rectangle
void fb_draw_rounded_rect(framebuffer_t* fb_ptr, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t radius, uint32_t color) {
    for (uint32_t py = y; py < y + h; py++) {
        for (uint32_t px = x; px < x + w; px++) {
            // Check if pixel is in rounded corner
            uint32_t dx = 0, dy = 0;
            
            if (px < x + radius && py < y + radius) {
                dx = x + radius - px;
                dy = y + radius - py;
            } else if (px >= x + w - radius && py < y + radius) {
                dx = px - (x + w - radius - 1);
                dy = y + radius - py;
            } else if (px < x + radius && py >= y + h - radius) {
                dx = x + radius - px;
                dy = py - (y + h - radius - 1);
            } else if (px >= x + w - radius && py >= y + h - radius) {
                dx = px - (x + w - radius - 1);
                dy = py - (y + h - radius - 1);
            }
            
            if (dx * dx + dy * dy <= radius * radius) {
                fb_put_pixel(fb_ptr, px, py, color);
            } else if (dx == 0 && dy == 0) {
                fb_put_pixel(fb_ptr, px, py, color);
            }
        }
    }
}
