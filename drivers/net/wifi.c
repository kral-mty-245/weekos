// WeeK OS - WiFi Driver
// RTL8188 / RTL8821 / Atheros / Intel WiFi desteği

#include "../../include/kernel.h"

// ============================================
// WiFi Durumları
// ============================================
typedef enum {
    WIFI_STATE_OFF = 0,
    WIFI_STATE_DISCONNECTED,
    WIFI_STATE_SCANNING,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_ERROR
} wifi_state_t;

// Güvenlik türleri
typedef enum {
    WIFI_SEC_NONE = 0,
    WIFI_SEC_WEP,
    WIFI_SEC_WPA,
    WIFI_SEC_WPA2,
    WIFI_SEC_WPA3
} wifi_security_t;

// WiFi ağı
typedef struct {
    char     ssid[64];
    uint8_t  bssid[6];
    int8_t   signal_strength;     // dBm (-30 ile -90 arası)
    uint8_t  channel;
    wifi_security_t security;
    bool     is_hidden;
    bool     is_connected;
} wifi_network_t;

// WiFi istatistikleri
typedef struct {
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t errors;
    uint32_t disconnects;
    uint32_t reconnects;
} wifi_stats_t;

// WiFi yapılandırması
typedef struct {
    char     connected_ssid[64];
    uint8_t  ip_address[4];
    uint8_t  gateway[4];
    uint8_t  subnet[4];
    uint8_t  dns1[4];
    uint8_t  dns2[4];
    bool     dhcp_enabled;
    bool     static_ip;
} wifi_config_t;

// ============================================
// WiFi Driver Durumu
// ============================================
typedef struct {
    wifi_state_t   state;
    bool           enabled;
    uint8_t        mac_address[6];
    char           driver_name[32];
    char           firmware_version[32];

    // Ağ listesi
    wifi_network_t networks[32];
    uint32_t       network_count;

    // Bağlı ağ
    wifi_network_t* connected_network;

    // İstatistikler
    wifi_stats_t   stats;

    // Yapılandırma
    wifi_config_t  config;

    // Sinyal gücü
    int8_t         signal_strength;
    uint32_t       link_speed;       // Mbps
    uint32_t       frequency;        // MHz

    // Proxy
    bool           proxy_enabled;
    char           proxy_address[128];
    uint32_t       proxy_port;
} wifi_driver_t;

static wifi_driver_t wifi = {
    .state = WIFI_STATE_OFF,
    .enabled = false,
    .network_count = 0,
    .connected_network = NULL,
    .signal_strength = -50,
    .link_speed = 72,
    .frequency = 2437,
    .dhcp_enabled = true
};

// ============================================
// WiFi Başlatma / Durdurma
// ============================================
void wifi_init(void) {
    // Driver bilgileri
    strcpy(wifi.driver_name, "WeeK WiFi Driver v1.0");
    strcpy(wifi.firmware_version, "2.0.1");

    // Sahte MAC adresi
    wifi.mac_address[0] = 0x00;
    wifi.mac_address[1] = 0x1A;
    wifi.mac_address[2] = 0x2B;
    wifi.mac_address[3] = 0x3C;
    wifi.mac_address[4] = 0x4D;
    wifi.mac_address[5] = 0x5E;

    wifi.state = WIFI_STATE_DISCONNECTED;
}

void wifi_enable(void) {
    wifi.enabled = true;
    wifi.state = WIFI_STATE_DISCONNECTED;
    wifi_scan_networks();
}

void wifi_disable(void) {
    wifi.enabled = false;
    wifi.state = WIFI_STATE_OFF;

    if (wifi.connected_network) {
        wifi.connected_network->is_connected = false;
        wifi.connected_network = NULL;
    }
}

// ============================================
// Ağ Tarama
// ============================================
void wifi_scan_networks(void) {
    if (!wifi.enabled) return;

    wifi.state = WIFI_STATE_SCANNING;
    wifi.network_count = 0;

    // Gerçek donanımda: management frame göndererek çevre AP'leri tarar
    // Simülasyon: Örnek ağlar
    const char* ssids[] = {
        "TURKSAT-KABLONET-1868-5",
        "TurkTelekom-5G",
        "Vodafone-Net",
        "Cafe_Free_WiFi",
        "Guest-Network",
        "WeeK-Home-5G",
        "AndroidAP",
        "iPhone-Asan"
    };

    int8_t signals[] = { -42, -55, -67, -73, -80, -35, -62, -78 };
    wifi_security_t secs[] = { WIFI_SEC_WPA2, WIFI_SEC_WPA3, WIFI_SEC_WPA2,
                               WIFI_SEC_NONE, WIFI_SEC_WPA2, WIFI_SEC_WPA3,
                               WIFI_SEC_WPA2, WIFI_SEC_WPA };

    for (int i = 0; i < 8; i++) {
        wifi_network_t* net = &wifi.networks[wifi.network_count];
        strcpy(net->ssid, ssids[i]);
        net->signal_strength = signals[i];
        net->security = secs[i];
        net->channel = 1 + (i % 11);
        net->is_hidden = false;
        net->is_connected = false;

        // BSSID (sahte)
        net->bssid[0] = 0xAA;
        net->bssid[1] = 0xBB;
        net->bssid[2] = 0xCC;
        net->bssid[3] = 0xDD;
        net->bssid[4] = 0xEE;
        net->bssid[5] = 0xFF - i;

        wifi.network_count++;
    }

    wifi.state = WIFI_STATE_DISCONNECTED;
}

// ============================================
// Ağ Bağlantısı
// ============================================
bool wifi_connect(const char* ssid, const char* password) {
    if (!wifi.enabled) return false;

    wifi.state = WIFI_STATE_CONNECTING;

    // Ağ bul
    for (uint32_t i = 0; i < wifi.network_count; i++) {
        if (strcmp(wifi.networks[i].ssid, ssid) == 0) {
            wifi_network_t* net = &wifi.networks[i];

            // Bağlantı simülasyonu (gerçek donanımda 4-way handshake)
            // WPA2/WPA3 ise handshake gerekli
            // WEP ise basit şifre kontrolü

            // Bağlantı başarılı
            net->is_connected = true;
            wifi.connected_network = net;
            wifi.state = WIFI_STATE_CONNECTED;

            // IP yapılandırması (DHCP)
            wifi.config.ip_address[0] = 192;
            wifi.config.ip_address[1] = 168;
            wifi.config.ip_address[2] = 1;
        }
    }

    return (wifi.state == WIFI_STATE_CONNECTED);
}

void wifi_disconnect(void) {
    if (wifi.connected_network) {
        wifi.connected_network->is_connected = false;
        wifi.connected_network = NULL;
    }
    wifi.state = WIFI_STATE_DISCONNECTED;
}

// ============================================
// WiFi Bilgileri
// ============================================
wifi_driver_t* wifi_get_driver(void) {
    return &wifi;
}

wifi_state_t wifi_get_state(void) {
    return wifi.state;
}

int8_t wifi_get_signal_strength(void) {
    return wifi.signal_strength;
}

uint32_t wifi_get_link_speed(void) {
    return wifi.link_speed;
}

bool wifi_is_connected(void) {
    return wifi.state == WIFI_STATE_CONNECTED;
}

// Sinyal gücü yüzdesi
uint8_t wifi_get_signal_percent(void) {
    // dBm -> yüzde dönüşümü
    int8_t dbm = wifi.signal_strength;
    if (dbm >= -50) return 100;
    if (dbm <= -100) return 0;
    return (uint8_t)(2 * (dbm + 100));
}
