#ifndef WEEKOS_WINE_H
#define WEEKOS_WINE_H

#include "../../include/types.h"

// WINE initializer
void wine_init(void);
void wine_shutdown(void);

// .exe running
uint32_t wine_run_exe(const char* exe_path, const char* args);
bool wine_kill_process(uint32_t pid);
wine_process_t* wine_get_process(uint32_t pid);
int wine_get_process_count(void);

// Windows version
void wine_set_windows_version(uint32_t version);
const char* wine_get_windows_version_name(void);

// Drives
bool wine_add_drive(char letter, const char* host_path);
const char* wine_get_drive_path(char letter);

// GPU
void wine_enable_dxvk(bool enable);
void wine_enable_esync(bool enable);

// Settings
void wine_set_enabled(bool enabled);
void wine_set_max_memory(uint32_t mb);
void wine_set_max_cpu(uint32_t percent);
void wine_set_network(bool enabled);

// Status
wine_config_t* wine_get_config(void);
bool wine_is_initialized(void);
bool wine_is_enabled(void);
const char* wine_get_version(void);

// Compatibility
wine_compat_result_t wine_check_compatibility(const char* exe_path);

// Winetricks
bool wine_install_winetricks(const char* component);

// UI
void wine_draw_settings(framebuffer_t* fb);

#endif
