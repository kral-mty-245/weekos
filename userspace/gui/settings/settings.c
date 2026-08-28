// WeeK OS - Settings Application
// Windows 11 style settings with all categories

#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// Settings Categories
// ============================================
typedef enum {
    SETTING_HOME = 0,
    SETTING_SYSTEM,
    SETTING_BLUETOOTH,
    SETTING_NETWORK,
    SETTING_PERSONALIZATION,
    SETTING_APPS,
    SETTING_ACCOUNTS,
    SETTING_TIME,
    SETTING_GAMING,
    SETTING_ACCESSIBILITY,
    SETTING_PRIVACY,
    SETTING_UPDATE
} setting_category_t;

// ============================================
// Settings State
// ============================================
typedef struct {
    setting_category_t current_category;
    bool dark_mode;
    bool night_light;
    uint32_t night_light_schedule_start;
    uint32_t night_light_schedule_end;
    bool defender_enabled;
    uint32_t defender_scan_level;  // 0=off, 1=basic, 2=full
    uint32_t wallpaper_mode;      // 0=light, 1=dark, 2=auto
    uint32_t accent_color;
    bool notifications_enabled;
    bool sounds_enabled;
    uint32_t brightness;
    uint32_t volume;
    bool wifi_enabled;
    bool bluetooth_enabled;
    bool airplane_mode;
    bool vpn_enabled;
    char hostname[64];
    char language[32];
} settings_state_t;

static settings_state_t settings = {
    .current_category = SETTING_HOME,
    .dark_mode = false,
    .night_light = false,
    .defender_enabled = true,
    .defender_scan_level = 2,
    .wallpaper_mode = 2,  // Auto
    .accent_color = 0x0078D4,
    .notifications_enabled = true,
    .brightness = 75,
    .volume = 50,
    .wifi_enabled = true,
    .bluetooth_enabled = true,
    .airplane_mode = false,
    .vpn_enabled = false,
    .hostname = "WeeK-PC",
    .language = "tr-TR"
};

// ============================================
// Settings Menu Items
// ============================================
typedef struct {
    char icon[32];
    char name[64];
    setting_category_t category;
} setting_menu_item_t;

static const setting_menu_item_t menu_items[] = {
    {"🏠", "Giris", SETTING_HOME},
    {"💻", "Sistem", SETTING_SYSTEM},
    {"🔵", "Bluetooth ve cihazlar", SETTING_BLUETOOTH},
    {"🌐", "Ag ve internet", SETTING_NETWORK},
    {"🎨", "Kisisellestirme", SETTING_PERSONALIZATION},
    {"📱", "Uygulamalar", SETTING_APPS},
    {"👤", "Hesaplar", SETTING_ACCOUNTS},
    {"⏰", "Zaman ve dil", SETTING_TIME},
    {"🎮", "Oyun", SETTING_GAMING},
    {"♿", "Erisilebilirlik", SETTING_ACCESSIBILITY},
    {"🔒", "Gizlilik ve guvenlik", SETTING_PRIVACY},
    {"🔄", "WeeK OS Update", SETTING_UPDATE}
};

#define MENU_COUNT 12

// ============================================
// Draw Settings UI
// ============================================
void settings_draw(framebuffer_t* fb) {
    // Clear screen
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0xF0F0F0);
    
    // Left sidebar (glass effect)
    fb_draw_glass_panel(fb, 0, 0, 280, fb->height, 0x40);
    
    // User profile section
    fb_fill_rect(fb, 20, 20, 48, 48, 0x0078D4);  // Avatar
    // Username
    fb_fill_rect(fb, 80, 20, 180, 16, 0x1A1A1A);  // Name placeholder
    fb_fill_rect(fb, 80, 42, 180, 12, 0x666666);  // Email placeholder
    
    // Menu items
    for (int i = 0; i < MENU_COUNT; i++) {
        int y = 100 + i * 40;
        
        // Highlight current category
        if (menu_items[i].category == settings.current_category) {
            fb_fill_rect(fb, 10, y, 260, 36, 0x0078D4);
        }
        
        // Icon placeholder
        fb_fill_rect(fb, 20, y + 8, 20, 20, 0x0078D4);
        
        // Menu item text (placeholder)
        fb_fill_rect(fb, 50, y + 10, 200, 14, 0x1A1A1A);
    }
    
    // Main content area
    int content_x = 300;
    int content_y = 20;
    
    switch (settings.current_category) {
        case SETTING_HOME:
            settings_draw_home(fb, content_x, content_y);
            break;
        case SETTING_SYSTEM:
            settings_draw_system(fb, content_x, content_y);
            break;
        case SETTING_PERSONALIZATION:
            settings_draw_personalization(fb, content_x, content_y);
            break;
        case SETTING_NETWORK:
            settings_draw_network(fb, content_x, content_y);
            break;
        case SETTING_PRIVACY:
            settings_draw_privacy(fb, content_x, content_y);
            break;
        case SETTING_UPDATE:
            settings_draw_update(fb, content_x, content_y);
            break;
        default:
            settings_draw_default(fb, content_x, content_y);
            break;
    }
}

// ============================================
// Settings Sub-pages
// ============================================
void settings_draw_home(framebuffer_t* fb, int x, int y) {
    // Title
    fb_fill_rect(fb, x, y, 300, 32, 0x1A1A1A);
    
    // Search bar
    fb_draw_glass_panel(fb, x, y + 50, 600, 40, 0x40);
    fb_fill_rect(fb, x + 10, y + 60, 20, 20, 0x666666);  // Search icon
    
    // Quick settings cards
    int card_x = x;
    int card_y = y + 120;
    
    // System card
    fb_draw_glass_panel(fb, card_x, card_y, 280, 80, 0x40);
    fb_fill_rect(fb, card_x + 10, card_y + 10, 24, 24, 0x0078D4);  // Icon
    
    // Network card
    fb_draw_glass_panel(fb, card_x + 300, card_y, 280, 80, 0x40);
    fb_fill_rect(fb, card_x + 310, card_y + 10, 24, 24, 0x0078D4);
    
    // Personalization card
    fb_draw_glass_panel(fb, card_x, card_y + 100, 280, 80, 0x40);
    fb_fill_rect(fb, card_x + 10, card_y + 110, 24, 24, 0x0078D4);
    
    // Apps card
    fb_draw_glass_panel(fb, card_x + 300, card_y + 100, 280, 80, 0x40);
    fb_fill_rect(fb, card_x + 310, card_y + 110, 24, 24, 0x0078D4);
}

void settings_draw_system(framebuffer_t* fb, int x, int y) {
    // Title
    fb_fill_rect(fb, x, y, 300, 32, 0x1A1A1A);
    
    // System settings list
    const char* items[] = {
        "Ekran",
        "Ses",
        "Bildirimler",
        "Guc ve pil",
        "Depolama",
        "Multimedya",
        "Yazi写字板",
        "Coklu gorev",
        "Tablet modu",
        "Projecting to this PC",
        "Yakalama",
        "Uzaktan masaustu",
        "Hizli bilgisayar"
    };
    
    for (int i = 0; i < 13; i++) {
        int item_y = y + 60 + i * 44;
        
        fb_draw_glass_panel(fb, x, item_y, 600, 40, 0x40);
        fb_fill_rect(fb, x + 10, item_y + 10, 20, 20, 0x0078D4);  // Icon
        // Item text (placeholder)
        fb_fill_rect(fb, x + 40, item_y + 12, 200, 14, 0x1A1A1A);
        // Arrow
        fb_fill_rect(fb, x + 570, item_y + 12, 8, 14, 0x666666);
    }
}

void settings_draw_personalization(framebuffer_t* fb, int x, int y) {
    // Title
    fb_fill_rect(fb, x, y, 400, 32, 0x1A1A1A);
    
    // Theme selection
    fb_fill_rect(fb, x, y + 50, 600, 200, 0x0078D4);  // Theme preview
    
    // Theme options
    const char* themes[] = {"Acik", "Karanlik", "Otomatik"};
    for (int i = 0; i < 3; i++) {
        int theme_x = x + i * 200;
        fb_fill_rect(fb, theme_x, y + 270, 180, 100, 0xF0F0F0);
        fb_fill_rect(fb, theme_x + 10, y + 280, 160, 80, i == 0 ? 0x0078D4 : 0xCCCCCC);
    }
    
    // Wallpaper section
    fb_fill_rect(fb, x, y + 400, 600, 32, 0x1A1A1A);  // Section title
    
    // Wallpaper thumbnails
    for (int i = 0; i < 4; i++) {
        fb_fill_rect(fb, x + i * 155, y + 450, 150, 100, 0x0078D4);
    }
    
    // Colors section
    fb_fill_rect(fb, x, y + 580, 600, 32, 0x1A1A1A);
    
    // Accent colors
    uint32_t colors[] = {0x0078D4, 0x0099BC, 0x7A7574, 0x767676, 0xFF8C00};
    for (int i = 0; i < 5; i++) {
        fb_fill_rect(fb, x + i * 40, y + 630, 32, 32, colors[i]);
    }
}

void settings_draw_network(framebuffer_t* fb, int x, int y) {
    fb_fill_rect(fb, x, y, 300, 32, 0x1A1A1A);
    
    // WiFi section
    fb_draw_glass_panel(fb, x, y + 60, 600, 40, 0x40);
    // WiFi toggle
    fb_fill_rect(fb, x + 550, y + 65, 40, 24, settings.wifi_enabled ? 0x0078D4 : 0x999999);
    
    // Bluetooth section
    fb_draw_glass_panel(fb, x, y + 110, 600, 40, 0x40);
    fb_fill_rect(fb, x + 550, y + 115, 40, 24, settings.bluetooth_enabled ? 0x0078D4 : 0x999999);
    
    // Airplane mode
    fb_draw_glass_panel(fb, x, y + 160, 600, 40, 0x40);
    fb_fill_rect(fb, x + 550, y + 165, 40, 24, settings.airplane_mode ? 0x0078D4 : 0x999999);
    
    // VPN
    fb_draw_glass_panel(fb, x, y + 210, 600, 40, 0x40);
    fb_fill_rect(fb, x + 550, y + 215, 40, 24, settings.vpn_enabled ? 0x0078D4 : 0x999999);
}

void settings_draw_privacy(framebuffer_t* fb, int x, int y) {
    fb_fill_rect(fb, x, y, 300, 32, 0x1A1A1A);
    
    // WeeK Defender section
    fb_draw_glass_panel(fb, x, y + 60, 600, 120, 0x40);
    fb_fill_rect(fb, x + 10, y + 70, 32, 32, settings.defender_enabled ? 0x0078D4 : 0xFF0000);
    
    // Defender toggle
    fb_fill_rect(fb, x + 550, y + 75, 40, 24, settings.defender_enabled ? 0x0078D4 : 0x999999);
    
    // Scan level
    fb_fill_rect(fb, x + 20, y + 120, 560, 40, 0xF0F0F0);
    // Scan buttons
    const char* scan_levels[] = {"Kapali", "Temel", "Tam Tarama"};
    for (int i = 0; i < 3; i++) {
        fb_fill_rect(fb, x + 30 + i * 180, y + 125, 160, 30, 
                     i == settings.defender_scan_level ? 0x0078D4 : 0xCCCCCC);
    }
    
    // Firewall
    fb_draw_glass_panel(fb, x, y + 200, 600, 40, 0x40);
    fb_fill_rect(fb, x + 550, y + 205, 40, 24, 0x0078D4);
    
    // App control
    fb_draw_glass_panel(fb, x, y + 250, 600, 40, 0x40);
}

void settings_draw_update(framebuffer_t* fb, int x, int y) {
    fb_fill_rect(fb, x, y, 400, 32, 0x1A1A1A);
    
    // Update status
    fb_draw_glass_panel(fb, x, y + 60, 600, 100, 0x40);
    fb_fill_rect(fb, x + 20, y + 80, 24, 24, 0x00C853);  // Checkmark
    
    // Check for updates button
    fb_fill_rect(fb, x + 20, y + 120, 200, 32, 0x0078D4);
    
    // Update history
    fb_draw_glass_panel(fb, x, y + 180, 600, 40, 0x40);
    
    // Advanced options
    fb_draw_glass_panel(fb, x, y + 230, 600, 40, 0x40);
    
    // Delivery optimization
    fb_draw_glass_panel(fb, x, y + 280, 600, 40, 0x40);
}

void settings_draw_default(framebuffer_t* fb, int x, int y) {
    fb_fill_rect(fb, x, y, 300, 32, 0x1A1A1A);
    fb_fill_rect(fb, x, y + 60, 600, 40, 0xF0F0F0);
}

// ============================================
// Settings Navigation
// ============================================
void settings_set_category(int cat) {
    settings.current_category = cat;
}

void settings_toggle_dark_mode(void) {
    settings.dark_mode = !settings.dark_mode;
}

void settings_toggle_defender(void) {
    settings.defender_enabled = !settings.defender_enabled;
}

void settings_toggle_wifi(void) {
    settings.wifi_enabled = !settings.wifi_enabled;
}

void settings_toggle_bluetooth(void) {
    settings.bluetooth_enabled = !settings.bluetooth_enabled;
}

void settings_toggle_night_light(void) {
    settings.night_light = !settings.night_light;
}

settings_state_t* settings_get_state(void) {
    return &settings;
}
