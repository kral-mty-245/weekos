// WeeK OS - Network Stack
// Basic network driver (placeholder)

#include "../../include/kernel.h"

// Network device
typedef struct {
    char     name[32];
    uint8_t  mac[6];
    uint32_t ip;
    uint32_t subnet;
    uint32_t gateway;
    bool     connected;
} net_device_t;

static net_device_t net_device;

// Initialize network
void net_init(void) {
    strcpy(net_device.name, "eth0");
    net_device.mac[0] = 0x00;
    net_device.mac[1] = 0x1A;
    net_device.mac[2] = 0x2B;
    net_device.mac[3] = 0x3C;
    net_device.mac[4] = 0x4D;
    net_device.mac[5] = 0x5E;
    net_device.ip = 0xC0A80001;  // 192.168.0.1
    net_device.subnet = 0xFFFFFF00;  // 255.255.255.0
    net_device.gateway = 0xC0A80001;  // 192.168.0.1
    net_device.connected = true;
}

// Get network device info
net_device_t* net_get_device(void) {
    return &net_device;
}
