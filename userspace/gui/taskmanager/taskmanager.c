// WeeK OS - Task Manager
// Windows 11 style process manager

#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// Process Info
// ============================================
typedef struct {
    pid_t     pid;
    char      name[64];
    uint32_t  cpu_usage;
    uint32_t  memory_usage;
    uint32_t  disk_usage;
    uint32_t  network_usage;
    uint32_t  status;  // 0=running, 1=suspended, 2=ending
    uint32_t  threads;
    uint32_t  handles;
    uint32_t  up_time;
} process_info_t;

#define MAX_PROCESSES 256
static process_info_t process_list[MAX_PROCESSES];
static int process_count = 0;
static int selected_tab = 0;  // 0=Processes, 1=Performance, 2=Startup, 3=Users, 4=Details, 5=Services

// ============================================
// Performance Data
// ============================================
typedef struct {
    uint32_t cpu_usage;
    uint32_t memory_used;
    uint32_t memory_total;
    uint32_t disk_read;
    uint32_t disk_write;
    uint32_t network_sent;
    uint32_t network_recv;
    uint32_t gpu_usage;
    uint32_t gpu_memory;
} performance_data_t;

static performance_data_t perf_data = {
    .cpu_usage = 15,
    .memory_used = 2048,
    .memory_total = 8192,
    .disk_read = 100,
    .disk_write = 50,
    .network_sent = 1024,
    .network_recv = 2048,
    .gpu_usage = 5,
    .gpu_memory = 256
};

// ============================================
// Task Manager UI
// ============================================
void taskmanager_draw(framebuffer_t* fb) {
    // Clear screen
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0xF0F0F0);
    
    // Title bar
    fb_fill_rect(fb, 0, 0, fb->width, 40, 0x0078D4);
    
    // Tab buttons
    const char* tabs[] = {"Surecler", "Performans", "Baslangic", 
                          "Kullanicilar", "Ayrintilar", "Hizmetler"};
    int tab_x = 20;
    for (int i = 0; i < 6; i++) {
        uint32_t bg = (i == selected_tab) ? 0x0078D4 : 0xE0E0E0;
        fb_fill_rect(fb, tab_x, 50, 120, 32, bg);
        tab_x += 130;
    }
    
    // Main content
    switch (selected_tab) {
        case 0: taskmanager_draw_processes(fb); break;
        case 1: taskmanager_draw_performance(fb); break;
        case 2: taskmanager_draw_startup(fb); break;
        case 3: taskmanager_draw_users(fb); break;
        case 4: taskmanager_draw_details(fb); break;
        case 5: taskmanager_draw_services(fb); break;
    }
    
    // Status bar
    fb_fill_rect(fb, 0, fb->height - 30, fb->width, 30, 0xE0E0E0);
    fb_fill_rect(fb, 10, fb->height - 25, 200, 16, 0x1A1A1A);
}

// ============================================
// Processes Tab
// ============================================
void taskmanager_draw_processes(framebuffer_t* fb) {
    int y = 90;
    
    // Column headers
    fb_fill_rect(fb, 20, y, fb->width - 40, 30, 0xE0E0E0);
    
    // Headers: Name, PID, CPU, Memory, Disk, Network, Status
    const char* headers[] = {"Ad", "PID", "CPU", "Bellek", "Disk", "Ag", "Durum"};
    int col_x = 30;
    int col_widths[] = {200, 60, 60, 80, 60, 60, 100};
    
    for (int i = 0; i < 7; i++) {
        fb_fill_rect(fb, col_x, y + 8, col_widths[i], 14, 0x1A1A1A);
        col_x += col_widths[i] + 10;
    }
    
    // Process list
    y += 35;
    for (int i = 0; i < MIN(process_count, 20); i++) {
        process_info_t* proc = &process_list[i];
        
        // Row background
        uint32_t row_bg = (i % 2 == 0) ? 0xFFFFFF : 0xF8F8F8;
        fb_fill_rect(fb, 20, y, fb->width - 40, 28, row_bg);
        
        // Process data
        col_x = 30;
        
        // Name
        fb_fill_rect(fb, col_x, y + 7, 200, 14, 0x1A1A1A);
        col_x += 210;
        
        // PID
        fb_fill_rect(fb, col_x, y + 7, 60, 14, 0x666666);
        col_x += 70;
        
        // CPU
        uint32_t cpu_color = proc->cpu_usage > 80 ? 0xFF0000 : 
                            proc->cpu_usage > 50 ? 0xFF9800 : 0x00C853;
        fb_fill_rect(fb, col_x, y + 7, 50, 14, cpu_color);
        col_x += 70;
        
        // Memory
        fb_fill_rect(fb, col_x, y + 7, 80, 14, 0x666666);
        col_x += 90;
        
        // Disk
        fb_fill_rect(fb, col_x, y + 7, 60, 14, 0x666666);
        col_x += 70;
        
        // Network
        fb_fill_rect(fb, col_x, y + 7, 60, 14, 0x666666);
        col_x += 70;
        
        // Status
        uint32_t status_color = proc->status == 0 ? 0x00C853 : 0xFF9800;
        fb_fill_rect(fb, col_x, y + 7, 100, 14, status_color);
        
        y += 28;
    }
    
    // End task button
    fb_fill_rect(fb, fb->width - 140, fb->height - 70, 120, 30, 0x0078D4);
}

// ============================================
// Performance Tab
// ============================================
void taskmanager_draw_performance(framebuffer_t* fb) {
    int y = 90;
    
    // CPU graph
    fb_draw_glass_panel(fb, 20, y, 300, 200, 0x40);
    fb_fill_rect(fb, 30, y + 10, 100, 16, 0x1A1A1A);
    
    // CPU usage graph (simplified bar chart)
    for (int i = 0; i < 50; i++) {
        int bar_height = (perf_data.cpu_usage * 150) / 100;
        fb_fill_rect(fb, 40 + i * 5, y + 180 - bar_height, 4, bar_height, 0x0078D4);
    }
    
    // Memory graph
    fb_draw_glass_panel(fb, 340, y, 300, 200, 0x40);
    fb_fill_rect(fb, 350, y + 10, 100, 16, 0x1A1A1A);
    
    // Memory usage bar
    uint32_t mem_pct = (perf_data.memory_used * 100) / perf_data.memory_total;
    fb_fill_rect(fb, 350, y + 40, 280, 20, 0xCCCCCC);
    fb_fill_rect(fb, 350, y + 40, 280 * mem_pct / 100, 20, 0x0078D4);
    
    // Disk graph
    fb_draw_glass_panel(fb, 20, y + 220, 300, 200, 0x40);
    fb_fill_rect(fb, 30, y + 230, 100, 16, 0x1A1A1A);
    
    // Network graph
    fb_draw_glass_panel(fb, 340, y + 220, 300, 200, 0x40);
    fb_fill_rect(fb, 350, y + 230, 100, 16, 0x1A1A1A);
    
    // GPU graph
    fb_draw_glass_panel(fb, 660, y, 300, 200, 0x40);
    fb_fill_rect(fb, 670, y + 10, 100, 16, 0x1A1A1A);
    
    // Ethernet graph
    fb_draw_glass_panel(fb, 660, y + 220, 300, 200, 0x40);
    fb_fill_rect(fb, 670, y + 230, 100, 16, 0x1A1A1A);
}

// ============================================
// Startup Tab
// ============================================
void taskmanager_draw_startup(framebuffer_t* fb) {
    int y = 90;
    
    // Column headers
    fb_fill_rect(fb, 20, y, fb->width - 40, 30, 0xE0E0E0);
    fb_fill_rect(fb, 30, y + 8, 200, 14, 0x1A1A1A);  // Name
    fb_fill_rect(fb, 250, y + 8, 100, 14, 0x1A1A1A);  // Publisher
    fb_fill_rect(fb, 370, y + 8, 100, 14, 0x1A1A1A);  // Status
    fb_fill_rect(fb, 490, y + 8, 100, 14, 0x1A1A1A);  // Startup impact
    
    // Startup items
    y += 35;
    const char* startup_items[] = {
        "WeeK Defender", "WeeK Update", "Bluetooth", 
        "OneDrive", "Teams", "Edge"
    };
    
    for (int i = 0; i < 6; i++) {
        fb_fill_rect(fb, 20, y, fb->width - 40, 28, i % 2 ? 0xF8F8F8 : 0xFFFFFF);
        fb_fill_rect(fb, 30, y + 7, 200, 14, 0x1A1A1A);
        fb_fill_rect(fb, 370, y + 7, 80, 14, 0x00C853);  // Enabled
        fb_fill_rect(fb, 490, y + 7, 80, 14, 0x666666);  // Impact
        y += 28;
    }
}

// ============================================
// Users Tab
// ============================================
void taskmanager_draw_users(framebuffer_t* fb) {
    int y = 90;
    
    fb_fill_rect(fb, 20, y, fb->width - 40, 30, 0xE0E0E0);
    fb_fill_rect(fb, 30, y + 8, 150, 14, 0x1A1A1A);  // User
    fb_fill_rect(fb, 200, y + 8, 100, 14, 0x1A1A1A);  // Status
    fb_fill_rect(fb, 320, y + 8, 100, 14, 0x1A1A1A);  // CPU
    
    y += 35;
    // Current user
    fb_fill_rect(fb, 20, y, fb->width - 40, 28, 0xFFFFFF);
    fb_fill_rect(fb, 30, y + 7, 150, 14, 0x1A1A1A);
    fb_fill_rect(fb, 200, y + 7, 80, 14, 0x00C853);  // Active
}

// ============================================
// Details Tab
// ============================================
void taskmanager_draw_details(framebuffer_t* fb) {
    int y = 90;
    
    fb_fill_rect(fb, 20, y, fb->width - 40, 30, 0xE0E0E0);
    
    const char* headers[] = {"Ad", "PID", "Durum", "Kullanici Adi", "CPU", "Bellek"};
    int col_x = 30;
    for (int i = 0; i < 6; i++) {
        fb_fill_rect(fb, col_x, y + 8, 120, 14, 0x1A1A1A);
        col_x += 130;
    }
    
    y += 35;
    for (int i = 0; i < MIN(process_count, 25); i++) {
        fb_fill_rect(fb, 20, y, fb->width - 40, 24, i % 2 ? 0xF8F8F8 : 0xFFFFFF);
        y += 24;
    }
}

// ============================================
// Services Tab
// ============================================
void taskmanager_draw_services(framebuffer_t* fb) {
    int y = 90;
    
    fb_fill_rect(fb, 20, y, fb->width - 40, 30, 0xE0E0E0);
    
    const char* headers[] = {"Ad", "Aciklama", "Durum", "Baslangic Tipi", "Giris Yapma"};
    int col_x = 30;
    for (int i = 0; i < 5; i++) {
        fb_fill_rect(fb, col_x, y + 8, 140, 14, 0x1A1A1A);
        col_x += 150;
    }
    
    y += 35;
    // System services
    const char* services[] = {
        "WeeK Defender", "WeeK Update", "WeeK Audio", 
        "WeeK Display", "WeeK Network", "WeeK Firewall"
    };
    
    for (int i = 0; i < 6; i++) {
        fb_fill_rect(fb, 20, y, fb->width - 40, 24, i % 2 ? 0xF8F8F8 : 0xFFFFFF);
        fb_fill_rect(fb, 30, y + 5, 140, 14, 0x1A1A1A);
        fb_fill_rect(fb, 330, y + 5, 80, 14, 0x00C853);  // Running
        y += 24;
    }
}

// ============================================
// Task Manager Actions
// ============================================
void taskmanager_set_tab(int tab) {
    selected_tab = tab;
}

void taskmanager_end_process(pid_t pid) {
    // Find and remove process
    for (int i = 0; i < process_count; i++) {
        if (process_list[i].pid == pid) {
            // Shift array
            for (int j = i; j < process_count - 1; j++) {
                process_list[j] = process_list[j + 1];
            }
            process_count--;
            break;
        }
    }
}

void taskmanager_update_performance(void) {
    // Update performance data
    // In real implementation, this would read from /proc
    perf_data.cpu_usage = 10 + (timer_get_ticks() % 30);
    perf_data.memory_used = 2000 + (timer_get_ticks() % 1000);
}

performance_data_t* taskmanager_get_performance(void) {
    return &perf_data;
}

process_info_t* taskmanager_get_processes(void) {
    return process_list;
}

int taskmanager_get_process_count(void) {
    return process_count;
}
