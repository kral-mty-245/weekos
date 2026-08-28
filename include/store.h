#ifndef WEEKOS_STORE_H
#define WEEKOS_STORE_H

#include "../../include/types.h"
#include "../../include/kernel.h"

// Store initializer
void store_init(void);

// App listing
store_app_t* store_get_all_apps(void);
uint32_t     store_get_app_count(void);
store_app_t* store_get_app_by_id(uint32_t id);

// Install / Uninstall
bool store_install_app(uint32_t app_id);
bool store_uninstall_app(uint32_t app_id);

// Developer
uint32_t store_register_developer(const char* name, const char* email,
                                   const char* password, const char* company);
bool store_developer_login(const char* email, const char* password);
void store_developer_logout(void);
bool store_developer_publish_app(const char* name, const char* description,
                                  const char* exe_path, app_category_t category,
                                  uint32_t permissions);

// Reviews
bool store_add_review(uint32_t app_id, const char* username,
                       uint8_t rating, const char* comment);

// UI
void store_draw_main(framebuffer_t* fb);
void store_draw_detail(framebuffer_t* fb, store_app_t* app);
void store_draw_install_dialog(framebuffer_t* fb, store_app_t* app);
void store_draw_dev_console(framebuffer_t* fb);

#endif
