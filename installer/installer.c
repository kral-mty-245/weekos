// WeeK OS - Installation Wizard
// Windows 11 style step-by-step installer

#include "../include/kernel.h"
#include "../include/types.h"

// ============================================
// Installer State
// ============================================
typedef enum {
    INST_LANG = 0,
    INST_REGION,
    INST_KEYBOARD,
    INST_LICENSE,
    INST_TYPE,
    INST_DISK,
    INST_ACCOUNT,
    INST_CUSTOMIZE,
    INST_INSTALLING,
    INST_COMPLETE
} install_step_t;

typedef struct {
    install_step_t current_step;
    char     language[32];
    char     region[32];
    char     keyboard_layout[32];
    bool     accepted_license;
    uint32_t install_type;  // 0=upgrade, 1=custom
    uint32_t disk_target;
    char     username[64];
    char     password[128];
    char     hostname[64];
    uint32_t theme;         // 0=light, 1=dark, 2=auto
    bool     express_settings;
    uint32_t install_progress;
    bool     installing;
    bool     complete;
} installer_state_t;

static installer_state_t installer = {
    .current_step = INST_LANG,
    .language = "Turkce",
    .region = "Turkiye",
    .keyboard_layout = "Turk Q",
    .accepted_license = false,
    .install_type = 1,
    .disk_target = 0,
    .username = "Kullanici",
    .hostname = "WeeK-PC",
    .theme = 2,
    .express_settings = true,
    .install_progress = 0,
    .installing = false,
    .complete = false
};

// ============================================
// Language Selection Screen
// ============================================
void installer_draw_language(framebuffer_t* fb) {
    // Blue background
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x0078D4);
    
    // Center card
    int card_w = 600;
    int card_h = 500;
    int card_x = (fb->width - card_w) / 2;
    int card_y = (fb->height - card_h) / 2;
    
    fb_fill_rect(fb, card_x, card_y, card_w, card_h, 0xFFFFFF);
    
    // Title
    fb_fill_rect(fb, card_x + 40, card_y + 40, 400, 32, 0x1A1A1A);
    
    // Language dropdown
    fb_fill_rect(fb, card_x + 40, card_y + 100, card_w - 80, 40, 0xF0F0F0);
    fb_fill_rect(fb, card_x + 40, card_y + 100, card_w - 80, 2, 0x0078D4);
    
    // Language list
    const char* languages[] = {
        "Turkce", "English", "Deutsch", "Francais", 
        "Espanol", "Italiano", "Portugues", "Nederlands"
    };
    
    for (int i = 0; i < 8; i++) {
        int item_y = card_y + 160 + i * 36;
        
        if (strcmp(languages[i], installer.language) == 0) {
            fb_fill_rect(fb, card_x + 40, item_y, card_w - 80, 32, 0x0078D4);
        } else {
            fb_fill_rect(fb, card_x + 40, item_y, card_w - 80, 32, 0xF0F0F0);
        }
        
        // Language name (placeholder)
        fb_fill_rect(fb, card_x + 60, item_y + 8, 200, 14, 0x1A1A1A);
    }
    
    // Next button
    fb_fill_rect(fb, card_x + card_w - 140, card_y + card_h - 60, 100, 36, 0x0078D4);
}

// ============================================
// Region Selection Screen
// ============================================
void installer_draw_region(framebuffer_t* fb) {
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x0078D4);
    
    int card_w = 600;
    int card_h = 400;
    int card_x = (fb->width - card_w) / 2;
    int card_y = (fb->height - card_h) / 2;
    
    fb_fill_rect(fb, card_x, card_y, card_w, card_h, 0xFFFFFF);
    
    // Title
    fb_fill_rect(fb, card_x + 40, card_y + 40, 400, 32, 0x1A1A1A);
    
    // Region list
    const char* regions[] = {
        "Turkiye", "Amerika Birlesik Devletleri", "Birlesik Krallik",
        "Almanya", "Fransa", "Italya", "Ispanya", "Rusya"
    };
    
    for (int i = 0; i < 8; i++) {
        int item_y = card_y + 100 + i * 36;
        
        if (strcmp(regions[i], installer.region) == 0) {
            fb_fill_rect(fb, card_x + 40, item_y, card_w - 80, 32, 0x0078D4);
        } else {
            fb_fill_rect(fb, card_x + 40, item_y, card_w - 80, 32, 0xF0F0F0);
        }
    }
    
    // Next button
    fb_fill_rect(fb, card_x + card_w - 140, card_y + card_h - 60, 100, 36, 0x0078D4);
}

// ============================================
// License Agreement Screen
// ============================================
void installer_draw_license(framebuffer_t* fb) {
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x0078D4);
    
    int card_w = 700;
    int card_h = 500;
    int card_x = (fb->width - card_w) / 2;
    int card_y = (fb->height - card_h) / 2;
    
    fb_fill_rect(fb, card_x, card_y, card_w, card_h, 0xFFFFFF);
    
    // Title
    fb_fill_rect(fb, card_x + 40, card_y + 40, 400, 32, 0x1A1A1A);
    
    // License text area
    fb_fill_rect(fb, card_x + 40, card_y + 100, card_w - 80, 300, 0xF8F8F8);
    
    // Scrollbar
    fb_fill_rect(fb, card_x + card_w - 60, card_y + 100, 20, 300, 0xE0E0E0);
    
    // Accept checkbox
    fb_fill_rect(fb, card_x + 40, card_y + 420, 20, 20, 
                 installer.accepted_license ? 0x0078D4 : 0xCCCCCC);
    
    // Next button (disabled if not accepted)
    uint32_t next_color = installer.accepted_license ? 0x0078D4 : 0xCCCCCC;
    fb_fill_rect(fb, card_x + card_w - 140, card_y + card_h - 60, 100, 36, next_color);
}

// ============================================
// Installation Type Screen
// ============================================
void installer_draw_type(framebuffer_t* fb) {
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x0078D4);
    
    int card_w = 700;
    int card_h = 400;
    int card_x = (fb->width - card_w) / 2;
    int card_y = (fb->height - card_h) / 2;
    
    fb_fill_rect(fb, card_x, card_y, card_w, card_h, 0xFFFFFF);
    
    // Title
    fb_fill_rect(fb, card_x + 40, card_y + 40, 400, 32, 0x1A1A1A);
    
    // Upgrade option
    fb_fill_rect(fb, card_x + 40, card_y + 100, card_w - 80, 80, 0xF0F0F0);
    fb_fill_rect(fb, card_x + 60, card_y + 120, 24, 24, 0x0078D4);
    
    // Custom option
    fb_fill_rect(fb, card_x + 40, card_y + 200, card_w - 80, 80, 0xF0F0F0);
    fb_fill_rect(fb, card_x + 60, card_y + 220, 24, 24, 0x0078D4);
    
    // Next button
    fb_fill_rect(fb, card_x + card_w - 140, card_y + card_h - 60, 100, 36, 0x0078D4);
}

// ============================================
// Account Creation Screen
// ============================================
void installer_draw_account(framebuffer_t* fb) {
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x0078D4);
    
    int card_w = 600;
    int card_h = 500;
    int card_x = (fb->width - card_w) / 2;
    int card_y = (fb->height - card_h) / 2;
    
    fb_fill_rect(fb, card_x, card_y, card_w, card_h, 0xFFFFFF);
    
    // Title
    fb_fill_rect(fb, card_x + 40, card_y + 40, 400, 32, 0x1A1A1A);
    
    // User avatar
    fb_fill_rect(fb, card_x + card_w/2 - 40, card_y + 80, 80, 80, 0x0078D4);
    
    // Username field
    fb_fill_rect(fb, card_x + 40, card_y + 180, card_w - 80, 40, 0xF0F0F0);
    fb_fill_rect(fb, card_x + 40, card_y + 180, card_w - 80, 2, 0x0078D4);
    
    // Password field
    fb_fill_rect(fb, card_x + 40, card_y + 240, card_w - 80, 40, 0xF0F0F0);
    fb_fill_rect(fb, card_x + 40, card_y + 240, card_w - 80, 2, 0x0078D4);
    
    // Confirm password
    fb_fill_rect(fb, card_x + 40, card_y + 300, card_w - 80, 40, 0xF0F0F0);
    fb_fill_rect(fb, card_x + 40, card_y + 300, card_w - 80, 2, 0x0078D4);
    
    // Next button
    fb_fill_rect(fb, card_x + card_w - 140, card_y + card_h - 60, 100, 36, 0x0078D4);
}

// ============================================
// Customization Screen
// ============================================
void installer_draw_customize(framebuffer_t* fb) {
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x0078D4);
    
    int card_w = 700;
    int card_h = 500;
    int card_x = (fb->width - card_w) / 2;
    int card_y = (fb->height - card_h) / 2;
    
    fb_fill_rect(fb, card_x, card_y, card_w, card_h, 0xFFFFFF);
    
    // Title
    fb_fill_rect(fb, card_x + 40, card_y + 40, 400, 32, 0x1A1A1A);
    
    // Theme selection
    fb_fill_rect(fb, card_x + 40, card_y + 100, 200, 100, 0xF0F0F0);  // Light
    fb_fill_rect(fb, card_x + 260, card_y + 100, 200, 100, 0x2D2D2D);  // Dark
    fb_fill_rect(fb, card_x + 480, card_y + 100, 200, 100, 0x0078D4);  // Blue
    
    // Accent colors
    uint32_t colors[] = {0x0078D4, 0x0099BC, 0x7A7574, 0x767676, 0xFF8C00, 0xE81123};
    for (int i = 0; i < 6; i++) {
        fb_fill_rect(fb, card_x + 40 + i * 40, card_y + 240, 32, 32, colors[i]);
    }
    
    // Privacy settings toggles
    for (int i = 0; i < 5; i++) {
        fb_fill_rect(fb, card_x + 40, card_y + 300 + i * 36, card_w - 80, 32, 0xF0F0F0);
        fb_fill_rect(fb, card_x + card_w - 80, card_y + 306 + i * 36, 40, 20, 0x0078D4);
    }
    
    // Next button
    fb_fill_rect(fb, card_x + card_w - 140, card_y + card_h - 60, 100, 36, 0x0078D4);
}

// ============================================
// Installing Screen
// ============================================
void installer_draw_installing(framebuffer_t* fb) {
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x0078D4);
    
    int card_w = 600;
    int card_h = 300;
    int card_x = (fb->width - card_w) / 2;
    int card_y = (fb->height - card_h) / 2;
    
    fb_fill_rect(fb, card_x, card_y, card_w, card_h, 0xFFFFFF);
    
    // Title
    fb_fill_rect(fb, card_x + 40, card_y + 40, 400, 32, 0x1A1A1A);
    
    // Status text
    fb_fill_rect(fb, card_x + 40, card_y + 100, 400, 20, 0x666666);
    
    // Progress bar
    fb_fill_rect(fb, card_x + 40, card_y + 140, card_w - 80, 24, 0xE0E0E0);
    fb_fill_rect(fb, card_x + 40, card_y + 140, 
                (card_w - 80) * installer.install_progress / 100, 24, 0x0078D4);
    
    // Percentage
    fb_fill_rect(fb, card_x + 40, card_y + 180, 100, 20, 0x1A1A1A);
    
    // Tips
    fb_fill_rect(fb, card_x + 40, card_y + 220, card_w - 80, 40, 0x666666);
}

// ============================================
// Complete Screen
// ============================================
void installer_draw_complete(framebuffer_t* fb) {
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x0078D4);
    
    int card_w = 600;
    int card_h = 300;
    int card_x = (fb->width - card_w) / 2;
    int card_y = (fb->height - card_h) / 2;
    
    fb_fill_rect(fb, card_x, card_y, card_w, card_h, 0xFFFFFF);
    
    // Success icon
    fb_fill_rect(fb, card_x + card_w/2 - 40, card_y + 30, 80, 80, 0x00C853);
    
    // Title
    fb_fill_rect(fb, card_x + 40, card_y + 130, card_w - 80, 32, 0x1A1A1A);
    
    // Message
    fb_fill_rect(fb, card_x + 40, card_y + 180, card_w - 80, 20, 0x666666);
    
    // Restart button
    fb_fill_rect(fb, card_x + card_w/2 - 60, card_y + card_h - 60, 120, 36, 0x0078D4);
}

// ============================================
// Main Installer Draw
// ============================================
void installer_draw(framebuffer_t* fb) {
    switch (installer.current_step) {
        case INST_LANG: installer_draw_language(fb); break;
        case INST_REGION: installer_draw_region(fb); break;
        case INST_LICENSE: installer_draw_license(fb); break;
        case INST_TYPE: installer_draw_type(fb); break;
        case INST_ACCOUNT: installer_draw_account(fb); break;
        case INST_CUSTOMIZE: installer_draw_customize(fb); break;
        case INST_INSTALLING: installer_draw_installing(fb); break;
        case INST_COMPLETE: installer_draw_complete(fb); break;
    }
}

// ============================================
// Installer Navigation
// ============================================
void installer_next_step(void) {
    if (installer.current_step == INST_INSTALLING) return;
    
    if (installer.current_step == INST_COMPLETE) {
        // Restart system
        return;
    }
    
    installer.current_step++;
}

void installer_prev_step(void) {
    if (installer.current_step > INST_LANG) {
        installer.current_step--;
    }
}

void installer_set_language(const char* lang) {
    strcpy(installer.language, lang);
}

void installer_set_region(const char* region) {
    strcpy(installer.region, region);
}

void installer_accept_license(void) {
    installer.accepted_license = !installer.accepted_license;
}

void installer_set_username(const char* username) {
    strcpy(installer.username, username);
}

void installer_set_password(const char* password) {
    strcpy(installer.password, password);
}

void installer_start_install(void) {
    installer.installing = true;
    installer.install_progress = 0;
}

installer_state_t* installer_get_state(void) {
    return &installer;
}
