// WeeK OS - Quick Settings Panel
// Windows 11 style action center with WiFi, Bluetooth, etc.

#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// Quick Settings State
// ============================================
typedef struct {
    bool wifi_enabled;
    bool bluetooth_enabled;
    bool airplane_mode;
    bool vpn_enabled;
    bool night_light;
    bool focus_mode;
    bool nearby_share;
    bool screen_cast;
    bool accessibility;
    bool studio_effects;
    
    uint32_t brightness;
    uint32_t volume;
    bool volume_muted;
    
    char wifi_name[64];
    char bluetooth_device[64];
    bool wifi_connected;
    bool bluetooth_connected;
    
    bool panel_visible;
} quick_settings_state_t;

static quick_settings_state_t qs = {
    .wifi_enabled = true,
    .bluetooth_enabled = true,
    .airplane_mode = false,
    .vpn_enabled = false,
    .night_light = false,
    .focus_mode = false,
    .nearby_share = false,
    .screen_cast = false,
    .accessibility = false,
    .studio_effects = false,
    .brightness = 75,
    .volume = 50,
    .volume_muted = false,
    .wifi_name = "TURKSAT-KABLONET-1868-5",
    .bluetooth_device = "BT5.4 Mouse",
    .wifi_connected = true,
    .bluetooth_connected = true,
    .panel_visible = false
};

// ============================================
// Quick Settings Panel UI
// ============================================
void quicksettings_draw(framebuffer_t* fb) {
    if (!qs.panel_visible) return;
    
    int panel_w = 360;
    int panel_h = 520;
    int panel_x = fb->width - panel_w - 20;
    int panel_y = fb->height - panel_h - 60;
    
    // Panel background (liquid glass)
    fb_draw_glass_panel(fb, panel_x, panel_y, panel_w, panel_h, 0x80);
    
    // Quick toggle buttons (2x3 grid)
    int btn_w = 100;
    int btn_h = 80;
    int btn_margin = 10;
    int btn_start_x = panel_x + 20;
    int btn_start_y = panel_y + 20;
    
    // Row 1: WiFi, Bluetooth, Studio Effects
    // WiFi button
    uint32_t wifi_bg = qs.wifi_enabled ? 0x0078D4 : 0x404040;
    fb_fill_rect(fb, btn_start_x, btn_start_y, btn_w, btn_h, wifi_bg);
    fb_fill_rect(fb, btn_start_x + 35, btn_start_y + 15, 30, 30, 0xFFFFFF);  // WiFi icon
    
    // WiFi status text
    if (qs.wifi_connected) {
        fb_fill_rect(fb, btn_start_x, btn_start_y + btn_h + 5, btn_w, 12, 0x666666);
    }
    
    // Bluetooth button
    uint32_t bt_bg = qs.bluetooth_enabled ? 0x0078D4 : 0x404040;
    fb_fill_rect(fb, btn_start_x + btn_w + btn_margin, btn_start_y, btn_w, btn_h, bt_bg);
    fb_fill_rect(fb, btn_start_x + btn_w + btn_margin + 35, btn_start_y + 15, 30, 30, 0xFFFFFF);
    
    // Studio Effects button
    fb_fill_rect(fb, btn_start_x + 2 * (btn_w + btn_margin), btn_start_y, btn_w, btn_h, 0x404040);
    fb_fill_rect(fb, btn_start_x + 2 * (btn_w + btn_margin) + 35, btn_start_y + 15, 30, 30, 0xFFFFFF);
    
    // Row 2: Airplane, Accessibility, VPN
    int row2_y = btn_start_y + btn_h + 30;
    
    // Airplane mode
    uint32_t airplane_bg = qs.airplane_mode ? 0x0078D4 : 0x404040;
    fb_fill_rect(fb, btn_start_x, row2_y, btn_w, btn_h, airplane_bg);
    fb_fill_rect(fb, btn_start_x + 35, row2_y + 15, 30, 30, 0xFFFFFF);
    
    // Accessibility
    fb_fill_rect(fb, btn_start_x + btn_w + btn_margin, row2_y, btn_w, btn_h, 0x404040);
    fb_fill_rect(fb, btn_start_x + btn_w + btn_margin + 35, row2_y + 15, 30, 30, 0xFFFFFF);
    
    // VPN
    uint32_t vpn_bg = qs.vpn_enabled ? 0x0078D4 : 0x404040;
    fb_fill_rect(fb, btn_start_x + 2 * (btn_w + btn_margin), row2_y, btn_w, btn_h, vpn_bg);
    fb_fill_rect(fb, btn_start_x + 2 * (btn_w + btn_margin) + 35, row2_y + 15, 30, 30, 0xFFFFFF);
    
    // Brightness slider
    int slider_y = row2_y + btn_h + 30;
    fb_fill_rect(fb, panel_x + 20, slider_y, panel_w - 40, 8, 0x666666);
    fb_fill_rect(fb, panel_x + 20, slider_y, (panel_w - 40) * qs.brightness / 100, 8, 0x0078D4);
    
    // Brightness icon
    fb_fill_rect(fb, panel_x + 10, slider_y - 8, 16, 16, 0xFFFFFF);
    
    // Volume slider
    int vol_y = slider_y + 30;
    fb_fill_rect(fb, panel_x + 20, vol_y, panel_w - 40, 8, 0x666666);
    fb_fill_rect(fb, panel_x + 20, vol_y, (panel_w - 40) * qs.volume / 100, 8, 0x0078D4);
    
    // Volume icon
    fb_fill_rect(fb, panel_x + 10, vol_y - 8, 16, 16, 0xFFFFFF);
    
    // Mute button
    fb_fill_rect(fb, panel_x + panel_w - 40, vol_y - 8, 24, 24, 
                 qs.volume_muted ? 0xFF0000 : 0x404040);
    
    // Battery indicator
    int batt_y = vol_y + 30;
    fb_fill_rect(fb, panel_x + 20, batt_y, 60, 24, 0x404040);
    fb_fill_rect(fb, panel_x + 22, batt_y + 2, 56, 20, 0x00C853);
    fb_fill_rect(fb, panel_x + 80, batt_y + 4, 40, 16, 0x666666);
    
    // Settings button
    fb_fill_rect(fb, panel_x + panel_w - 50, panel_y + panel_h - 40, 32, 32, 0x404040);
}

// ============================================
// Quick Settings Actions
// ============================================
void quicksettings_toggle_wifi(void) {
    qs.wifi_enabled = !qs.wifi_enabled;
    if (!qs.wifi_enabled) {
        qs.wifi_connected = false;
    }
}

void quicksettings_toggle_bluetooth(void) {
    qs.bluetooth_enabled = !qs.bluetooth_enabled;
    if (!qs.bluetooth_enabled) {
        qs.bluetooth_connected = false;
    }
}

void quicksettings_toggle_airplane(void) {
    qs.airplane_mode = !qs.airplane_mode;
    if (qs.airplane_mode) {
        qs.wifi_enabled = false;
        qs.bluetooth_enabled = false;
        qs.wifi_connected = false;
        qs.bluetooth_connected = false;
    }
}

void quicksettings_toggle_vpn(void) {
    qs.vpn_enabled = !qs.vpn_enabled;
}

void quicksettings_toggle_night_light(void) {
    qs.night_light = !qs.night_light;
}

void quicksettings_toggle_mute(void) {
    qs.volume_muted = !qs.volume_muted;
}

void quicksettings_set_brightness(uint32_t brightness) {
    qs.brightness = brightness;
}

void quicksettings_set_volume(uint32_t volume) {
    qs.volume = volume;
}

void quicksettings_show(void) {
    qs.panel_visible = true;
}

void quicksettings_hide(void) {
    qs.panel_visible = false;
}

quick_settings_state_t* quicksettings_get_state(void) {
    return &qs;
}
