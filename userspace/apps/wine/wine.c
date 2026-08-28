// WeeK OS - WINE Integration
// Windows .exe dosyalarını çalıştırma desteği

#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// WINE Yapılandırması
// ============================================
#define WINE_PREFIX      "/home/user/.wine"
#define WINE_DRIVE_C     "/home/user/.wine/drive_c"
#define WINE_MAX_APPS    64
#define WINE_MAX_PATH    512

// WINE sürücü haritalaması
typedef struct {
    char letter;           // C:, D:, E: vb.
    char host_path[WINE_MAX_PATH];
    char wine_path[WINE_MAX_PATH];
} wine_drive_t;

// Çalıştırılan .exe bilgisi
typedef struct {
    uint32_t   pid;
    char       exe_path[WINE_MAX_PATH];
    char       wine_prefix[WINE_MAX_PATH];
    char       app_name[128];
    bool       running;
    uint32_t   memory_usage;
    uint32_t   cpu_usage;
    uint32_t   start_time;
    uint32_t   windows_version;  // Windows sürümü emülasyonu
} wine_process_t;

// WINE yapılandırması
typedef struct {
    bool        enabled;
    bool        initialized;
    char        version[32];         // WINE sürümü
    char        prefix[WINE_MAX_PATH];

    // Sürücü haritalaması
    wine_drive_t drives[26];
    int         drive_count;

    // Çalışan süreçler
    wine_process_t processes[WINE_MAX_APPS];
    int         process_count;

    // Windows sürüm emülasyonu
    uint32_t    windows_version;     // 0=win7, 1=win8, 2=win8.1, 3=win10, 4=win11

    // OpenGL / GPU desteği
    bool        gpu_enabled;
    bool        dxvk_enabled;        // DirectX Vulkan çevirici
    bool        esync_enabled;       // Enhanced synchronization

    // Ses
    bool        pulseaudio_enabled;
    bool        alsa_enabled;

    // Ağ
    bool        network_enabled;
    bool        proxy_enabled;
    char        proxy_address[256];

    // Kaynak sınırları
    uint32_t    max_memory;          // Maksimum bellek (MB)
    uint32_t    max_cpu;             // Maksimum CPU (%)

    // Uyumluluk modları
    bool        winetricks_enabled;
    char        override_dlls[64][128];
    int         override_count;
} wine_config_t;

static wine_config_t wine = {
    .enabled = true,
    .initialized = false,
    .version = "9.14",
    .windows_version = 3,  // Windows 10
    .gpu_enabled = true,
    .dxvk_enabled = true,
    .esync_enabled = true,
    .pulseaudio_enabled = true,
    .network_enabled = true,
    .max_memory = 4096,
    .max_cpu = 100,
    .drive_count = 0,
    .process_count = 0
};

// ============================================
// Windows Sürüm Numaraları
// ============================================
typedef enum {
    WIN_VER_WINDOWS_7   = 0x0601,
    WIN_VER_WINDOWS_8   = 0x0602,
    WIN_VER_WINDOWS_8_1 = 0x0603,
    WIN_VER_WINDOWS_10  = 0x0A00,
    WIN_VER_WINDOWS_11  = 0x0A01
} windows_version_t;

// ============================================
// WINE Başlatma
// ============================================
void wine_init(void) {
    if (wine.initialized) return;

    // WINE prefix dizinini oluştur
    // Gerçek implementasyonda: mkdir -p
    strcpy(wine.prefix, WINE_PREFIX);

    // Sürücü haritalaması
    wine.drives[0].letter = 'C';
    strcpy(wine.drives[0].host_path, WINE_DRIVE_C);
    strcpy(wine.drives[0].wine_path, "C:\\");

    wine.drives[1].letter = 'D';
    strcpy(wine.drives[1].host_path, "/media");
    strcpy(wine.drives[1].wine_path, "D:\\");

    wine.drives[2].letter = 'Z';
    strcpy(wine.drives[2].host_path, "/");
    strcpy(wine.drives[2].wine_path, "Z:\\");

    wine.drive_count = 3;

    // Registry ayarlarını başlat
    // wine_registry_init();

    // DLL yüklemelerini başlat
    // wine_dll_init();

    wine.initialized = true;
}

void wine_shutdown(void) {
    // Çalışan tüm süreci kapat
    for (int i = 0; i < wine.process_count; i++) {
        if (wine.processes[i].running) {
            wine_kill_process(wine.processes[i].pid);
        }
    }

    wine.process_count = 0;
    wine.initialized = false;
}

// ============================================
// .exe Çalıştırma
// ============================================
uint32_t wine_run_exe(const char* exe_path, const char* args) {
    if (!wine.initialized) wine_init();
    if (wine.process_count >= WINE_MAX_APPS) return 0;

    wine_process_t* proc = &wine.processes[wine.process_count];

    // Proses bilgilerini doldur
    proc->pid = wine.process_count + 1000;
    strcpy(proc->exe_path, exe_path);
    strcpy(proc->wine_prefix, wine.prefix);
    proc->running = true;
    proc->memory_usage = 0;
    proc->cpu_usage = 0;
    proc->start_time = timer_get_ticks();
    proc->windows_version = wine.windows_version;

    // Uygulama adını çıkar
    const char* last_slash = strrchr(exe_path, '/');
    if (!last_slash) last_slash = strrchr(exe_path, '\\');
    if (last_slash) {
        strcpy(proc->app_name, last_slash + 1);
    } else {
        strcpy(proc->app_name, exe_path);
    }

    wine.process_count++;

    return proc->pid;
}

// ============================================
// Proses Yönetimi
// ============================================
uint8_t wine_kill_process(uint32_t pid) {
    for (int i = 0; i < wine.process_count; i++) {
        if (wine.processes[i].pid == pid) {
            wine.processes[i].running = false;

            // Listeden kaldır
            for (int j = i; j < wine.process_count - 1; j++) {
                wine.processes[j] = wine.processes[j + 1];
            }
            wine.process_count--;

            return true;
        }
    }
    return false;
}

wine_process_t* wine_get_process(uint32_t pid) {
    for (int i = 0; i < wine.process_count; i++) {
        if (wine.processes[i].pid == pid) {
            return &wine.processes[i];
        }
    }
    return NULL;
}

int wine_get_process_count(void) {
    return wine.process_count;
}

wine_process_t* wine_get_all_processes(void) {
    return wine.processes;
}

// ============================================
// DLL Yönetimi
// ============================================
bool wine_override_dll(const char* dll_name, const char* builtin_type) {
    if (wine.override_count >= 64) return false;

    char override[256];
    sprintf(override, "%s=%s", dll_name, builtin_type);

    strcpy(wine.override_dlls[wine.override_count], override);
    wine.override_count++;

    return true;
}

// ============================================
// Windows Sürüm Emülasyonu
// ============================================
void wine_set_windows_version(uint32_t version) {
    wine.windows_version = version;

    // Registry'yi güncelle
    // wine_registry_set("Software\\Microsoft\\Windows NT\\CurrentVersion",
    //                   "CurrentVersion", version);
}

const char* wine_get_windows_version_name(void) {
    switch (wine.windows_version) {
        case 0: return "Windows 7";
        case 1: return "Windows 8";
        case 2: return "Windows 8.1";
        case 3: return "Windows 10";
        case 4: return "Windows 11";
        default: return "Bilinmeyen";
    }
}

// ============================================
// Sürücü Yönetimi
// ============================================
bool wine_add_drive(char letter, const char* host_path) {
    if (wine.drive_count >= 26) return false;

    wine_drive_t* drive = &wine.drives[wine.drive_count];
    drive->letter = letter;
    strcpy(drive->host_path, host_path);
    sprintf(drive->wine_path, "%c:\\", letter);

    wine.drive_count++;
    return true;
}

const char* wine_get_drive_path(char letter) {
    for (int i = 0; i < wine.drive_count; i++) {
        if (wine.drives[i].letter == letter) {
            return wine.drives[i].host_path;
        }
    }
    return NULL;
}

// ============================================
// Winetricks Desteği
// ============================================
bool wine_install_winetricks(const char* component) {
    if (!wine.winetricks_enabled) return false;

    // Winetricks ile bileşen kurulumu
    // Örnek: wine_install_winetricks("dotnet48")
    //        wine_install_winetricks("vcrun2019")
    //        wine_install_winetricks("dxvk")

    return true;
}

// ============================================
// GPU / DirectX Desteği
// ============================================
void wine_enable_dxvk(bool enable) {
    wine.dxvk_enabled = enable;

    if (enable) {
        // DXVK DLL'lerini yükle
        // dxvk_dxgi.dll, dxvk_d3d11.dll, vb.
    }
}

void wine_enable_esync(bool enable) {
    wine.esync_enabled = enable;
}

// ============================================
// WINE Ayarları
// ============================================
void wine_set_enabled(bool enabled) {
    wine.enabled = enabled;
    if (!enabled) {
        wine_shutdown();
    }
}

void wine_set_max_memory(uint32_t mb) {
    wine.max_memory = mb;
}

void wine_set_max_cpu(uint32_t percent) {
    wine.max_cpu = percent;
}

void wine_set_network(bool enabled) {
    wine.network_enabled = enabled;
}

// ============================================
// WINE Durumu
// ============================================
wine_config_t* wine_get_config(void) {
    return &wine;
}

bool wine_is_initialized(void) {
    return wine.initialized;
}

bool wine_is_enabled(void) {
    return wine.enabled;
}

const char* wine_get_version(void) {
    return wine.version;
}

// ============================================
// Uygulama Uyumluluk Kontrolü
// ============================================
typedef struct {
    bool compatible;
    char reason[256];
    uint32_t required_version;
    bool requires_dxvk;
    bool requires_dotnet;
    bool requires_vcrun;
} wine_compat_result_t;

wine_compat_result_t wine_check_compatibility(const char* exe_path) {
    wine_compat_result_t result;
    result.compatible = true;
    result.requires_dxvk = false;
    result.requires_dotnet = false;
    result.requires_vcrun = false;
    result.required_version = WIN_VER_WINDOWS_10;

    // PE header analizi (basit)
    // Gerçek implementasyonda: exe'nin PE header'ını okuyarak
    // gerekli DLL'leri ve API'leri kontrol eder

    // Oyunlar genellikle DXVK gerektirir
    const char* ext = strrchr(exe_path, '.');
    if (ext && strcmp(ext, ".exe") == 0) {
        // Basit kontrol: dosya adında "game" veya "steam" varsa
        if (strstr(exe_path, "game") || strstr(exe_path, "steam")) {
            result.requires_dxvk = true;
        }

        // .NET uygulamaları
        if (strstr(exe_path, "dotnet") || strstr(exe_path, "netframework")) {
            result.requires_dotnet = true;
        }

        // Visual C++ çalıştırılabilirleri
        if (strstr(exe_path, "vcredist") || strstr(exe_path, "vc++")) {
            result.requires_vcrun = true;
        }
    }

    return result;
}

// ============================================
// WINE UI Çizim
// ============================================
void wine_draw_settings(framebuffer_t* fb) {
    int y = 80;

    // Başlık
    fb_fill_rect(fb, 0, 0, fb->width, 60, 0x0078D4);
    fb_fill_rect(fb, 20, 15, 250, 24, 0xFFFFFF);

    // Genel ayarlar
    fb_draw_glass_panel(fb, 20, y, fb->width - 40, 50, 0x40);
    fb_fill_rect(fb, 30, y + 10, 200, 16, 0x1A1A1A);

    // WINE sürümü
    fb_fill_rect(fb, fb->width - 150, y + 10, 120, 16, 0x0078D4);

    y += 70;

    // Windows sürümü seçimi
    fb_draw_glass_panel(fb, 20, y, fb->width - 40, 50, 0x40);
    fb_fill_rect(fb, 30, y + 10, 200, 16, 0x1A1A1A);

    // Versiyon butonları
    const char* versions[] = {"Win 7", "Win 8", "Win 8.1", "Win 10", "Win 11"};
    for (int i = 0; i < 5; i++) {
        uint32_t bg = (i == wine.windows_version) ? 0x0078D4 : 0xCCCCCC;
        fb_fill_rect(fb, 300 + i * 100, y + 8, 90, 30, bg);
    }

    y += 70;

    // DXVK
    fb_draw_glass_panel(fb, 20, y, fb->width - 40, 50, 0x40);
    fb_fill_rect(fb, 30, y + 10, 200, 16, 0x1A1A1A);
    fb_fill_rect(fb, fb->width - 100, y + 12, 50, 24,
                 wine.dxvk_enabled ? 0x0078D4 : 0x999999);

    y += 60;

    // ESync
    fb_draw_glass_panel(fb, 20, y, fb->width - 40, 50, 0x40);
    fb_fill_rect(fb, 30, y + 10, 200, 16, 0x1A1A1A);
    fb_fill_rect(fb, fb->width - 100, y + 12, 50, 24,
                 wine.esync_enabled ? 0x0078D4 : 0x999999);

    y += 60;

    // Sürücü haritalaması
    fb_draw_glass_panel(fb, 20, y, fb->width - 40, 120, 0x40);
    fb_fill_rect(fb, 30, y + 10, 200, 16, 0x1A1A1A);

    for (int i = 0; i < wine.drive_count; i++) {
        fb_fill_rect(fb, 40, y + 35 + i * 25, 300, 20, 0xF0F0F0);
    }

    y += 140;

    // Çalışan .exe'ler
    if (wine.process_count > 0) {
        fb_fill_rect(fb, 20, y, 200, 20, 0x1A1A1A);
        y += 30;

        for (int i = 0; i < wine.process_count; i++) {
            fb_draw_glass_panel(fb, 20, y, fb->width - 40, 40, 0x40);
            fb_fill_rect(fb, 30, y + 10, 200, 16, 0x1A1A1A);
            // Durdur butonu
            fb_fill_rect(fb, fb->width - 120, y + 6, 80, 28, 0xFF0000);
            y += 45;
        }
    }
}
