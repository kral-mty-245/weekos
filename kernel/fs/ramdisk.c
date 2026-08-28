// WeeK OS - RAM Disk
// In-memory filesystem for initrd

#include "../../include/kernel.h"

#define RAMDISK_SIZE (4 * 1024 * 1024)  // 4MB
#define RAMDISK_BASE 0x1000000           // 16MB

static uint8_t* ramdisk_data = (uint8_t*)RAMDISK_BASE;
static uint32_t ramdisk_size = RAMDISK_SIZE;
static int ramdisk_initialized = 0;

// Initialize ramdisk
void ramdisk_init(void) {
    ramdisk_data = (uint8_t*)RAMDISK_BASE;
    ramdisk_size = RAMDISK_SIZE;
    ramdisk_initialized = 1;
}

// Read from ramdisk
int ramdisk_read(uint32_t offset, void* buffer, uint32_t size) {
    if (!ramdisk_initialized) return -1;
    if (offset + size > ramdisk_size) return -1;
    
    memcpy(buffer, ramdisk_data + offset, size);
    return size;
}

// Write to ramdisk
int ramdisk_write(uint32_t offset, const void* buffer, uint32_t size) {
    if (!ramdisk_initialized) return -1;
    if (offset + size > ramdisk_size) return -1;
    
    memcpy(ramdisk_data + offset, buffer, size);
    return size;
}

// Get ramdisk info
uint32_t ramdisk_get_size(void) {
    return ramdisk_size;
}

uint8_t* ramdisk_get_data(void) {
    return ramdisk_data;
}
