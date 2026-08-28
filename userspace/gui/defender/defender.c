// WeeK Defender - Security & Antivirus
// Real-time protection, scanning, threat detection

#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// Defender State
// ============================================
typedef struct {
    bool     enabled;
    bool     realtime_protection;
    bool     firewall_enabled;
    bool     app_control_enabled;  // Akil Uygulama Denetimi
    uint32_t scan_level;           // 0=off, 1=quick, 2=full
    uint32_t threats_found;
    uint32_t files_scanned;
    uint32_t last_scan_time;
    char     last_scan_date[32];
    bool     is_scanning;
    uint32_t scan_progress;
} defender_state_t;

static defender_state_t defender = {
    .enabled = true,
    .realtime_protection = true,
    .firewall_enabled = true,
    .app_control_enabled = true,
    .scan_level = 2,
    .threats_found = 0,
    .files_scanned = 0,
    .last_scan_time = 0,
    .is_scanning = false,
    .scan_progress = 0
};

// ============================================
// Threat Database (simplified)
// ============================================
typedef struct {
    char     name[128];
    char     type[32];    // virus, trojan, malware, spyware
    uint32_t severity;    // 1=low, 2=medium, 3=high, 4=critical
    bool     detected;
} threat_t;

#define MAX_THREATS 256
static threat_t threats[MAX_THREATS];
static int threat_count = 0;

// ============================================
// Defender UI
// ============================================
void defender_draw(framebuffer_t* fb) {
    // Clear screen
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0xF0F0F0);
    
    // Header
    fb_fill_rect(fb, 0, 0, fb->width, 60, 0x0078D4);
    // Shield icon
    fb_fill_rect(fb, 20, 15, 32, 32, 0xFFFFFF);
    // Title
    fb_fill_rect(fb, 60, 20, 200, 20, 0xFFFFFF);
    
    // Status card
    int y = 80;
    fb_draw_glass_panel(fb, 20, y, fb->width - 40, 100, 0x40);
    
    // Status icon (green check or red X)
    if (defender.enabled) {
        fb_fill_rect(fb, 40, y + 20, 48, 48, 0x00C853);  // Green
    } else {
        fb_fill_rect(fb, 40, y + 20, 48, 48, 0xFF0000);  // Red
    }
    
    // Status text
    fb_fill_rect(fb, 100, y + 25, 300, 20, 0x1A1A1A);
    fb_fill_rect(fb, 100, y + 55, 400, 16, 0x666666);
    
    // Quick actions
    y = 200;
    
    // Real-time protection toggle
    fb_draw_glass_panel(fb, 20, y, (fb->width - 60) / 2, 60, 0x40);
    fb_fill_rect(fb, 30, y + 10, 24, 24, defender.realtime_protection ? 0x00C853 : 0xFF0000);
    fb_fill_rect(fb, 60, y + 15, 200, 14, 0x1A1A1A);
    // Toggle
    fb_fill_rect(fb, (fb->width - 60) / 2 - 60, y + 18, 40, 24, 
                 defender.realtime_protection ? 0x0078D4 : 0x999999);
    
    // Firewall toggle
    int x2 = (fb->width - 60) / 2 + 40;
    fb_draw_glass_panel(fb, x2, y, (fb->width - 60) / 2, 60, 0x40);
    fb_fill_rect(fb, x2 + 10, y + 10, 24, 24, defender.firewall_enabled ? 0x00C853 : 0xFF0000);
    fb_fill_rect(fb, x2 + 40, y + 15, 200, 14, 0x1A1A1A);
    fb_fill_rect(fb, x2 + (fb->width - 60) / 2 - 60, y + 18, 40, 24,
                 defender.firewall_enabled ? 0x0078D4 : 0x999999);
    
    y = 280;
    
    // Scan buttons
    fb_fill_rect(fb, 20, y, 200, 40, 0x0078D4);  // Quick Scan
    fb_fill_rect(fb, 240, y, 200, 40, 0x0078D4);  // Full Scan
    fb_fill_rect(fb, 460, y, 200, 40, 0x0078D4);  // Custom Scan
    
    // Scan progress (if scanning)
    if (defender.is_scanning) {
        y = 340;
        fb_draw_glass_panel(fb, 20, y, fb->width - 40, 60, 0x40);
        
        // Progress bar
        fb_fill_rect(fb, 40, y + 35, fb->width - 80, 20, 0xCCCCCC);
        fb_fill_rect(fb, 40, y + 35, 
                    (fb->width - 80) * defender.scan_progress / 100, 20, 0x0078D4);
        
        // Progress text
        fb_fill_rect(fb, 40, y + 15, 200, 14, 0x1A1A1A);
    }
    
    // Threat history
    y = 420;
    fb_fill_rect(fb, 20, y, 200, 20, 0x1A1A1A);  // Section title
    
    // Threat list
    for (int i = 0; i < MIN(threat_count, 5); i++) {
        int item_y = y + 30 + i * 40;
        fb_draw_glass_panel(fb, 20, item_y, fb->width - 40, 36, 0x40);
        
        // Threat severity indicator
        uint32_t severity_color;
        switch (threats[i].severity) {
            case 1: severity_color = 0xFFC107; break;  // Yellow
            case 2: severity_color = 0xFF9800; break;  // Orange
            case 3: severity_color = 0xFF5722; break;  // Deep Orange
            case 4: severity_color = 0xF44336; break;  // Red
            default: severity_color = 0x999999;
        }
        fb_fill_rect(fb, 30, item_y + 8, 8, 20, severity_color);
        
        // Threat name (placeholder)
        fb_fill_rect(fb, 50, item_y + 10, 300, 14, 0x1A1A1A);
        
        // Action buttons
        fb_fill_rect(fb, fb->width - 200, item_y + 6, 80, 24, 0xFF0000);  // Delete
        fb_fill_rect(fb, fb->width - 110, item_y + 6, 80, 24, 0x0078D4);  // Quarantine
    }
}

// ============================================
// Threat Detection
// ============================================

// Check file for known threats
int defender_scan_file(const char* filepath) {
    // Simplified threat detection
    // In real implementation, this would check file signatures,
    // heuristics, and behavior patterns
    
    defender.files_scanned++;
    
    // Check against threat database
    for (int i = 0; i < threat_count; i++) {
        if (strcmp(threats[i].name, filepath) == 0) {
            threats[i].detected = true;
            return 1;  // Threat found
        }
    }
    
    return 0;  // Clean
}

// Show threat alert (red screen)
void defender_show_threat_alert(framebuffer_t* fb, const char* filename) {
    // Full screen red alert
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0xFF0000);
    
    // Warning icon
    fb_fill_rect(fb, fb->width/2 - 48, 100, 96, 96, 0xFFFFFF);
    
    // Warning text
    fb_fill_rect(fb, fb->width/2 - 200, 220, 400, 32, 0xFFFFFF);
    
    // File name
    fb_fill_rect(fb, fb->width/2 - 300, 280, 600, 24, 0xFFFFFF);
    
    // "BU DOSYA TEHLIKELI" message
    fb_fill_rect(fb, fb->width/2 - 250, 320, 500, 40, 0xFFFFFF);
    
    // EVET button
    fb_fill_rect(fb, fb->width/2 - 120, 400, 100, 40, 0x0078D4);
    
    // HAYIR button
    fb_fill_rect(fb, fb->width/2 + 20, 400, 100, 40, 0xFFFFFF);
    
    // Description
    fb_fill_rect(fb, fb->width/2 - 300, 460, 600, 60, 0xFFFFFF);
}

// Handle threat response
void defender_handle_threat(framebuffer_t* fb, const char* filename, bool delete_file) {
    if (delete_file) {
        // Delete the malicious file
        // In real kernel, this would call unlink()
        defender.threats_found++;
        
        // Log the action
        // threat_log_add(filename, "DELETED");
    } else {
        // User chose to keep - monitor the file
        // Set up behavior monitoring
        // threat_log_add(filename, "MONITORED");
    }
}

// ============================================
// Real-time Protection
// ============================================
void defender_realtime_check(const char* filepath) {
    if (!defender.enabled || !defender.realtime_protection) return;
    
    // Quick signature check
    int result = defender_scan_file(filepath);
    
    if (result) {
        // Threat detected! Show alert
        // defender_show_threat_alert(current_fb, filepath);
    }
}

// ============================================
// Defender Settings
// ============================================
void defender_set_enabled(bool enabled) {
    defender.enabled = enabled;
}

void defender_set_realtime(bool enabled) {
    defender.realtime_protection = enabled;
}

void defender_set_firewall(bool enabled) {
    defender.firewall_enabled = enabled;
}

void defender_set_app_control(bool enabled) {
    defender.app_control_enabled = enabled;
}

defender_state_t* defender_get_state(void) {
    return &defender;
}
