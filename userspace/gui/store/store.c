// WeeK Store - Uygulama Mağazası
// Tam mağaza sistemi: uygulama listesi, detay sayfası, geliştirici hesabı, .exe kurulumu

#include "../../../include/kernel.h"
#include "../../../include/types.h"

// ============================================
// Tipler ve Sabitler
// ============================================
#define MAX_STORE_APPS       128
#define MAX_CATEGORIES       16
#define MAX_REVIEWS          50
#define MAX_DEVELOPERS       64
#define MAX_INSTALLED_APPS   64
#define MAX_SCREENSHOTS      6
#define MAX_APP_NAME         64
#define MAX_APP_DESC         512
#define MAX_APP_VERSION      16
#define MAX_DEVELOPER_NAME   64
#define MAX_DEVELOPER_EMAIL  128
#define MAX_APP_PATH         256

// Uygulama durumları
typedef enum {
    APP_STATE_NOT_INSTALLED = 0,
    APP_STATE_DOWNLOADING,
    APP_STATE_INSTALLING,
    APP_STATE_INSTALLED,
    APP_STATE_UPDATING,
    APP_STATE_UNINSTALLING,
    APP_STATE_ERROR
} app_state_t;

// Uygulama türleri
typedef enum {
    APP_TYPE_EXE    = 0,  // Windows .exe (WINE ile çalışır)
    APP_TYPE_NATIVE = 1,  // WeeK OS native uygulama
    APP_TYPE_WEB    = 2,  // Web tabanlı uygulama
    APP_TYPE_GAME   = 3   // Oyun
} app_type_t;

// Kategoriler
typedef enum {
    CAT_TUMU = 0,
    CAT_TARAYICILAR,
    CAT_GELISTIRME,
    CAT_SOSYAL,
    CAT_MUZIK_VIDEO,
    CAT_OYUNLAR,
    CAT_OFIS,
    CAT_GUVENLIK,
    CAT_ARACLAR,
    CAT_EGITIM,
    CAT_FOTOGRAF,
    CAT_SAGLIK,
    CAT_FINANS,
    CAT_ULASIM,
    CAT_YASAM
} app_category_t;

// İzin türleri
typedef enum {
    PERM_NETWORK     = 0x01,
    PERM_FILE_SYSTEM  = 0x02,
    PERM_CAMERA      = 0x04,
    PERM_MICROPHONE  = 0x08,
    PERM_LOCATION    = 0x10,
    PERM_CONTACTS    = 0x20,
    PERM_NOTIFICATIONS = 0x40,
    PERM_ADMIN       = 0x80
} app_permission_t;

// Değerlendirme
typedef struct {
    char     username[64];
    uint8_t  rating;       // 1-5
    char     comment[256];
    uint32_t date;
    bool     helpful;
} app_review_t;

// Ekran görüntüsü
typedef struct {
    char     path[MAX_APP_PATH];
    char     caption[128];
} app_screenshot_t;

// ============================================
// Geliştirici Hesabı
// ============================================
typedef struct {
    uint32_t id;
    char     name[MAX_DEVELOPER_NAME];
    char     email[MAX_DEVELOPER_EMAIL];
    char     password_hash[64];
    char     company[128];
    char     website[256];
    char     description[512];
    char     logo_path[MAX_APP_PATH];
    uint32_t verified;        // Doğrulanmış geliştirici
    uint32_t total_apps;
    uint32_t total_downloads;
    uint32_t rating_sum;
    uint32_t rating_count;
    uint32_t created_date;
    bool     is_active;
} developer_account_t;

// ============================================
// Uygulama Bilgisi
// ============================================
typedef struct {
    uint32_t id;
    char     name[MAX_APP_NAME];
    char     description[MAX_APP_DESC];
    char     short_desc[128];
    char     version[MAX_APP_VERSION];
    char     icon_path[MAX_APP_PATH];
    char     exe_path[MAX_APP_PATH];       // .exe dosyası yolu
    char     install_path[MAX_APP_PATH];   // Kurulum yolu

    app_type_t     type;
    app_category_t category;
    app_state_t    state;

    uint32_t developer_id;
    char     developer_name[MAX_DEVELOPER_NAME];

    // Dosya bilgileri
    uint32_t file_size;            // Byte cinsinden
    uint32_t download_count;
    uint32_t install_count;

    // Derecelendirme
    uint32_t rating_sum;           // Toplam puan
    uint32_t rating_count;         // Toplam değerlendirme sayısı
    float    avg_rating;           // Ortalama puan

    // İzinler (bit mask)
    uint32_t permissions;

    // Ekran görüntüleri
    app_screenshot_t screenshots[MAX_SCREENSHOTS];
    uint32_t screenshot_count;

    // Yorumlar
    app_review_t reviews[MAX_REVIEWS];
    uint32_t review_count;

    // Sürüm geçmişi
    char     changelog[1024];

    // Uyumluluk
    char     min_os_version[16];
    bool     requires_wine;        // WINE gerektiriyor mu?

    // Fiyat
    bool     is_free;
    uint32_t price_cents;          // Kuruş cinsinden (0 = ücretsiz)

    // Yaş sınırı
    uint8_t  age_rating;           // 0, 3, 7, 12, 16, 18

    // Güncelleme
    bool     has_update;
    char     new_version[MAX_APP_VERSION];

    // Mağaza görünümü
    bool     featured;
    bool     editor_choice;
    uint32_t featured_order;
} store_app_t;

// ============================================
// Mağaza Durumu
// ============================================
typedef struct {
    // Uygulamalar
    store_app_t    apps[MAX_STORE_APPS];
    uint32_t       app_count;

    // Geliştiriciler
    developer_account_t developers[MAX_DEVELOPERS];
    uint32_t       developer_count;

    // Kurulu uygulamalar
    uint32_t       installed_apps[MAX_INSTALLED_APPS];
    uint32_t       installed_count;

    // Aktif geliştirici (giriş yapan)
    int32_t        current_developer;   // -1 = giriş yok

    // Arama durumu
    char           search_query[128];
    app_category_t filter_category;
    bool           filter_free;
    bool           filter_installed;
    uint8_t        filter_min_rating;

    // Sayfalama
    uint32_t       current_page;
    uint32_t       items_per_page;

    // UI durumu
    uint32_t       selected_app_id;
    bool           show_detail;
    bool           show_install_dialog;
    bool           show_dev_console;
    bool           show_settings;

    // İndirme durumu
    uint32_t       downloading_app_id;
    uint32_t       download_progress;
    bool           is_downloading;

    // Sistem bilgisi
    uint32_t       total_storage;
    uint32_t       used_storage;
} store_state_t;

static store_state_t store = {
    .app_count = 0,
    .developer_count = 0,
    .installed_count = 0,
    .current_developer = -1,
    .filter_category = CAT_TUMU,
    .current_page = 0,
    .items_per_page = 20,
    .selected_app_id = 0,
    .show_detail = false,
    .total_storage = 50 * 1024 * 1024 * 1024,  // 50GB
    .used_storage = 5 * 1024 * 1024 * 1024       // 5GB
};

// ============================================
// Kategori İsimleri
// ============================================
static const char* category_names[] = {
    "Tümü",
    "Tarayıcılar",
    "Geliştirme",
    "Sosyal",
    "Müzik & Video",
    "Oyunlar",
    "Ofis",
    "Güvenlik",
    "Araçlar",
    "Eğitim",
    "Fotoğraf",
    "Sağlık",
    "Finans",
    "Ulaşım",
    "Yaşam"
};

// İzin isimleri
static const char* permission_names[] = {
    "İnternet Erişimi",
    "Dosya Sistemi Erişimi",
    "Kamera",
    "Mikrofon",
    "Konum",
    "Kişiler",
    "Bildirimler",
    "Yönetici Yetkisi"
};

// ============================================
// Hazır Uygulama Veritabanı
// ============================================
static void store_init_default_apps(void) {
    // ---- TARAYICILAR ----
    store_add_app("Google Chrome", "Dünyanın en popüler web tarayıcısı. Hızlı, güvenli ve özelleştirilebilir.", 
                  "Google Chrome, hızlı ve güvenli bir web tarayıcısıdır. Google hesabınızla senkronize ederek "
                  "yer imlerinizi, şifrelerinizi ve geçmişinizi tüm cihazlarınızda kullanabilirsiniz.",
                  "128.0.6613.85", APP_TYPE_EXE, CAT_TARAYICILAR, "Google LLC",
                  95 * 1024 * 1024, 2500000, 4.5f, PERM_NETWORK | PERM_FILE_SYSTEM | PERM_NOTIFICATIONS,
                  true, 0, 0, "3.0", true, 0, 12, true, "");

    store_add_app("Mozilla Firefox", "Gizlilik odaklı, açık kaynaklı web tarayıcısı.",
                  "Mozilla Firefox, gizliliğinizi koruyan açık kaynaklı bir web tarayıcısıdır. "
                  "Gelişmiş gizlilik korumaları, reklam engelleyici ve binlerce eklenti desteği sunar.",
                  "129.0", APP_TYPE_EXE, CAT_TARAYICILAR, "Mozilla Foundation",
                  80 * 1024 * 1024, 1800000, 4.4f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 12, true, "");

    store_add_app("Opera", "VPN özellikli, hız odaklı web tarayıcısı.",
                  "Opera, dahili VPN, reklam engelleyici ve Akıllı Çubuk ile donatılmış hızlı bir tarayıcı.",
                  "113.0.5230.47", APP_TYPE_EXE, CAT_TARAYICILAR, "Opera Software",
                  75 * 1024 * 1024, 900000, 4.3f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 12, true, "");

    // ---- GELİŞTİRME ----
    store_add_app("Visual Studio Code", "Microsoft'un güçlü kod düzenleme aracı.",
                  "Visual Studio Code, binlerce eklenti desteği olan ücretsiz ve güçlü bir kod editörü. "
                  "Python, JavaScript, C++, Java ve daha birçok dili destekler.",
                  "1.92.1", APP_TYPE_EXE, CAT_GELISTIRME, "Microsoft Corporation",
                  350 * 1024 * 1024, 3200000, 4.7f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("GitHub Desktop", "Git versiyon kontrol sistemi için masaüstü istemcisi.",
                  "GitHub Desktop ile pull request'leri kolayca yönetin, commit'lerinizi görselleştirin.",
                  "3.4.3", APP_TYPE_EXE, CAT_GELISTIRME, "GitHub Inc.",
                  120 * 1024 * 1024, 850000, 4.5f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("Node.js", "JavaScript runtime ortamı.",
                  "Node.js, JavaScript'i sunucu tarafında çalıştırmanızı sağlayan ücretsiz ve açık kaynaklı bir platformdur.",
                  "20.16.0", APP_TYPE_EXE, CAT_GELISTIRME, "OpenJS Foundation",
                  45 * 1024 * 1024, 1200000, 4.6f, PERM_NETWORK | PERM_FILE_SYSTEM | PERM_ADMIN,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("Python", "Python programlama dili ve pip paket yöneticisi.",
                  "Python 3.x çalışma zamanı ve pip paket yöneticisi. Yapay zeka, web ve veri bilimi için ideal.",
                  "3.12.5", APP_TYPE_EXE, CAT_GELISTIRME, "Python Software Foundation",
                  55 * 1024 * 1024, 1500000, 4.8f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("Git", "Dağıtık versiyon kontrol sistemi.",
                  "Git, dünyanın en popüler versiyon kontrol sistemidir. Hızlı, verimli ve güvenilir.",
                  "2.46.0", APP_TYPE_EXE, CAT_GELISTIRME, "Software Freedom Conservancy",
                  65 * 1024 * 1024, 2000000, 4.9f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    // ---- SOSYAL ----
    store_add_app("Discord", "Oyuncular ve topluluklar için sesli, görüntülü ve yazılı sohbet.",
                  "Discord, oyun toplulukları için tasarlanmış ücretsiz sesli, görüntülü ve yazılı sohbet uygulamasıdır.",
                  "1.0.9042", APP_TYPE_EXE, CAT_SOSYAL, "Discord Inc.",
                  180 * 1024 * 1024, 4000000, 4.6f, PERM_NETWORK | PERM_MICROPHONE | PERM_NOTIFICATIONS,
                  true, 0, 0, "3.0", true, 0, 13, true, "");

    store_add_app("Telegram Desktop", "Hızlı ve güvenli mesajlaşma uygulaması.",
                  "Telegram, bulut tabanlı, hızlı ve güvenli bir mesajlaşma uygulamasıdır. "
                  "200.000 kişilik grupları, kanalları ve botları destekler.",
                  "5.2.3", APP_TYPE_EXE, CAT_SOSYAL, "Telegram FZ-LLC",
                  55 * 1024 * 1024, 2800000, 4.7f, PERM_NETWORK | PERM_NOTIFICATIONS,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("WhatsApp Desktop", "Dünyanın en çok kullanılan mesajlaşma uygulaması.",
                  "WhatsApp Desktop ile telefonunuzdaki mesajlarınızı bilgisayarınızdan okuyun ve yanıtlayın.",
                  "2.2432.14.0", APP_TYPE_EXE, CAT_SOSYAL, "Meta Platforms Inc.",
                  110 * 1024 * 1024, 3500000, 4.3f, PERM_NETWORK | PERM_MICROPHONE | PERM_CAMERA,
                  true, 0, 0, "3.0", true, 0, 13, true, "");

    // ---- MÜZİK & VİDEO ----
    store_add_app("Spotify", "Dünyanın en büyük müzik ve podcast kütüphanesi.",
                  "Spotify, 100 milyonu aşkın şarkı ve podcast'i dinlemenizi sağlayan müzik platformu.",
                  "1.2.42.426", APP_TYPE_EXE, CAT_MUZIK_VIDEO, "Spotify AB",
                  140 * 1024 * 1024, 5000000, 4.5f, PERM_NETWORK | PERM_NOTIFICATIONS,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("VLC Media Player", "Evrensel medya oynatıcı. Her formatı çalıştırır.",
                  "VLC, neredeyse tüm ses ve video formatlarını oynatabilen ücretsiz ve açık kaynaklı medya oynatıcısı.",
                  "3.0.21", APP_TYPE_EXE, CAT_MUZIK_VIDEO, "VideoLAN",
                  45 * 1024 * 1024, 3000000, 4.8f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("OBS Studio", "Profesyonel ekran kayıt ve canlı yayın aracı.",
                  "OBS Studio, ücretsiz ve açık kaynaklı video kayıt ve canlı yayın yazılımıdır.",
                  "30.2.1", APP_TYPE_EXE, CAT_MUZIK_VIDEO, "OBS Project",
                  120 * 1024 * 1024, 1800000, 4.7f, PERM_NETWORK | PERM_FILE_SYSTEM | PERM_CAMERA | PERM_MICROPHONE,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    // ---- OYUNLAR ----
    store_add_app("Steam", "Dünyanın en büyük PC oyun platformu.",
                  "Steam, 50.000'den fazla oyunu indirebileceğiniz, topluluk etkinliklerine katılabileceğiniz "
                  "oyun platformu.",
                  "2.10.91.91", APP_TYPE_EXE, CAT_OYUNLAR, "Valve Corporation",
                  300 * 1024 * 1024, 8000000, 4.4f, PERM_NETWORK | PERM_FILE_SYSTEM | PERM_NOTIFICATIONS,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("Epic Games Store", "Epic Games'in oyun mağazası.",
                  "Her hafta ücretsiz oyunlar, büyük indirimler ve Epic Games'in kendi oyunları.",
                  "15.17.0", APP_TYPE_EXE, CAT_OYUNLAR, "Epic Games Inc.",
                  250 * 1024 * 1024, 2000000, 4.2f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("Minecraft Launcher", "Dünyanın en çok satan oyunu.",
                  "Minecraft ile sonsuz bir dünyada hayal gücünüzü kullanarak inşa edin ve hayatta kalın.",
                  "3.8.20", APP_TYPE_EXE, CAT_OYUNLAR, "Mojang Studios",
                  200 * 1024 * 1024, 6000000, 4.5f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 7, true, "");

    // ---- OFİS ----
    store_add_app("LibreOffice", "Ücretsiz ve açık kaynaklı ofis paketi.",
                  "LibreOffice, Writer, Calc, Impress ve daha birçok araç içeren eksiksiz bir ofis paketi.",
                  "24.8.0", APP_TYPE_NATIVE, CAT_OFIS, "The Document Foundation",
                  400 * 1024 * 1024, 1500000, 4.3f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", false, 0, 0, true, "");

    store_add_app("Notion", "Not alma, veritabanı ve proje yönetimi aracı.",
                  "Notion, notlarınızı, projelerinizi ve wiki'lerinizi tek bir yerde yönetmenizi sağlar.",
                  "3.14.0", APP_TYPE_EXE, CAT_OFIS, "Notion Labs Inc.",
                  180 * 1024 * 1024, 900000, 4.6f, PERM_NETWORK | PERM_FILE_SYSTEM | PERM_NOTIFICATIONS,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    // ---- GÜVENLİK ----
    store_add_app("Bitwarden", "Açık kaynaklı şifre yöneticisi.",
                  "Bitwarden, şifrelerinizi güvenli bir şekilde saklayan ücretsiz ve açık kaynaklı şifre yöneticisi.",
                  "2024.8.0", APP_TYPE_EXE, CAT_GUVENLIK, "Bitwarden Inc.",
                  95 * 1024 * 1024, 700000, 4.8f, PERM_NETWORK | PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("Malwarebytes", "Zararlı yazılım temizleme aracı.",
                  "Malwarebytes, bilgisayarınızı zararlı yazılımlardan koruyan ve temizleyen güvenlik aracı.",
                  "5.1.2", APP_TYPE_EXE, CAT_GUVENLIK, "Malwarebytes Inc.",
                  250 * 1024 * 1024, 1200000, 4.5f, PERM_NETWORK | PERM_FILE_SYSTEM | PERM_ADMIN,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    // ---- ARAÇLAR ----
    store_add_app("7-Zip", "Ücretsiz dosya sıkıştırma aracı.",
                  "7-Zip, yüksek sıkıştırma oranına sahip ücretsiz dosya arşivleyici.",
                  "24.08", APP_TYPE_NATIVE, CAT_ARACLAR, "Igor Pavlov",
                  2 * 1024 * 1024, 5000000, 4.9f, PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", false, 0, 0, true, "");

    store_add_app("PowerToys", "Windows için gelişmiş araçlar seti.",
                  "Microsoft PowerToys, Windows kullanıcıları için gelişmiş üretkenlik araçları sunar.",
                  "0.83.0", APP_TYPE_EXE, CAT_ARACLAR, "Microsoft Corporation",
                  250 * 1024 * 1024, 800000, 4.5f, PERM_FILE_SYSTEM | PERM_ADMIN,
                  true, 0, 0, "3.0", true, 0, 0, true, "");

    store_add_app("GIMP", "Profesyonel görüntü düzenleme aracı.",
                  "GIMP, ücretsiz ve açık kaynaklı Photoshop alternatifi görüntü düzenleyici.",
                  "2.10.38", APP_TYPE_NATIVE, CAT_FOTOGRAF, "GIMP Development Team",
                  300 * 1024 * 1024, 1100000, 4.4f, PERM_FILE_SYSTEM,
                  true, 0, 0, "3.0", false, 0, 0, true, "");

    store_add_app("Blender", "3D modelleme, animasyon ve video düzenleme.",
                  "Blender, ücretsiz ve açık kaynaklı 3D oluşturma aracı. Film, oyun ve tasarım için.",
                  "4.2.0", APP_TYPE_EXE, CAT_FOTOGRAF, "Blender Foundation",
                  450 * 1024 * 1024, 600000, 4.7f, PERM_FILE_SYSTEM | PERM_NETWORK,
                  true, 0, 0, "3.0", true, 0, 0, true, "");
}

// ============================================
// Uygulama Ekleme
// ============================================
static uint32_t store_next_app_id = 1;

void store_add_app(const char* name, const char* short_desc, const char* description,
                   const char* version, app_type_t type, app_category_t category,
                   const char* developer, uint32_t file_size, uint32_t downloads,
                   float rating, uint32_t permissions, bool is_free, uint32_t price,
                   uint32_t age_rating, bool requires_wine, bool featured,
                   const char* changelog) {
    if (store.app_count >= MAX_STORE_APPS) return;

    store_app_t* app = &store.apps[store.app_count];
    memset(app, 0, sizeof(store_app_t));

    app->id = store_next_app_id++;
    strcpy(app->name, name);
    strcpy(app->short_desc, short_desc);
    strcpy(app->description, description);
    strcpy(app->version, version);
    app->type = type;
    app->category = category;
    app->state = APP_STATE_NOT_INSTALLED;
    strcpy(app->developer_name, developer);
    app->file_size = file_size;
    app->download_count = downloads;
    app->rating_sum = (uint32_t)(rating * 10);
    app->rating_count = (uint32_t)(rating * 10);
    app->avg_rating = rating;
    app->permissions = permissions;
    app->requires_wine = requires_wine;
    app->is_free = is_free;
    app->price_cents = price;
    app->age_rating = age_rating;
    app->featured = featured;
    if (changelog) strcpy(app->changelog, changelog);

    store.app_count++;
}

// ============================================
// Mağaza Başlatma
// ============================================
void store_init(void) {
    memset(&store, 0, sizeof(store_state_t));
    store.filter_category = CAT_TUMU;
    store.items_per_page = 20;
    store.current_developer = -1;
    store.total_storage = 50UL * 1024 * 1024 * 1024;
    store.used_storage = 5UL * 1024 * 1024 * 1024;

    store_init_default_apps();
}

// ============================================
// Arama ve Filtreleme
// ============================================
static int store_filter_apps(store_app_t** results, int max_results) {
    int count = 0;

    for (uint32_t i = 0; i < store.app_count && count < max_results; i++) {
        store_app_t* app = &store.apps[i];

        // Kategori filtresi
        if (store.filter_category != CAT_TUMU && app->category != store.filter_category)
            continue;

        // Arama filtresi
        if (store.search_query[0] != '\0') {
            bool found = false;
            // İsimde ara
            if (strstr(app->name, store.search_query)) found = true;
            // Açıklamada ara
            if (strstr(app->short_desc, store.search_query)) found = true;
            if (!found) continue;
        }

        // Ücretsiz filtresi
        if (store.filter_free && !app->is_free) continue;

        // Kurulu filtresi
        if (store.filter_installed && app->state != APP_STATE_INSTALLED) continue;

        // Minimum puan filtresi
        if (store.filter_min_rating > 0 && app->avg_rating < store.filter_min_rating) continue;

        results[count++] = app;
    }

    return count;
}

// ============================================
// Uygulama Kurma
// ============================================
bool store_install_app(uint32_t app_id) {
    for (uint32_t i = 0; i < store.app_count; i++) {
        if (store.apps[i].id == app_id) {
            store_app_t* app = &store.apps[i];

            if (app->state == APP_STATE_INSTALLED) return false;
            if (store.installed_count >= MAX_INSTALLED_APPS) return false;

            // Depolama kontrolü
            if (store.used_storage + app->file_size > store.total_storage) return false;

            // Kurulum başlat
            app->state = APP_STATE_INSTALLING;
            store.downloading_app_id = app_id;
            store.is_downloading = true;
            store.download_progress = 0;

            // Simüle edilmiş kurulum
            app->state = APP_STATE_INSTALLED;
            app->install_count++;
            store.installed_apps[store.installed_count++] = app_id;
            store.used_storage += app->file_size;
            store.is_downloading = false;

            return true;
        }
    }
    return false;
}

// ============================================
// Uygulama Kaldırma
// ============================================
bool store_uninstall_app(uint32_t app_id) {
    for (uint32_t i = 0; i < store.app_count; i++) {
        if (store.apps[i].id == app_id) {
            store_app_t* app = &store.apps[i];

            if (app->state != APP_STATE_INSTALLED) return false;

            app->state = APP_STATE_NOT_INSTALLED;
            store.used_storage -= app->file_size;

            // Kurulu listesinden kaldır
            for (uint32_t j = 0; j < store.installed_count; j++) {
                if (store.installed_apps[j] == app_id) {
                    store.installed_apps[j] = store.installed_apps[store.installed_count - 1];
                    store.installed_count--;
                    break;
                }
            }

            return true;
        }
    }
    return false;
}

// ============================================
// Geliştirici Hesap İşlemleri
// ============================================
uint32_t store_register_developer(const char* name, const char* email, const char* password,
                                  const char* company) {
    if (store.developer_count >= MAX_DEVELOPERS) return 0;

    developer_account_t* dev = &store.developers[store.developer_count];
    memset(dev, 0, sizeof(developer_account_t));

    dev->id = store.developer_count + 1;
    strcpy(dev->name, name);
    strcpy(dev->email, email);
    strcpy(dev->company, company);
    dev->verified = false;
    dev->is_active = true;
    dev->total_apps = 0;
    dev->total_downloads = 0;

    store.developer_count++;
    return dev->id;
}

bool store_developer_login(const char* email, const char* password) {
    for (uint32_t i = 0; i < store.developer_count; i++) {
        if (strcmp(store.developers[i].email, email) == 0 && store.developers[i].is_active) {
            store.current_developer = i;
            return true;
        }
    }
    return false;
}

void store_developer_logout(void) {
    store.current_developer = -1;
}

bool store_developer_publish_app(const char* name, const char* description,
                                  const char* exe_path, app_category_t category,
                                  uint32_t permissions) {
    if (store.current_developer < 0) return false;

    developer_account_t* dev = &store.developers[store.current_developer];

    store_app_t* app = &store.apps[store.app_count];
    memset(app, 0, sizeof(store_app_t));

    app->id = store_next_app_id++;
    strcpy(app->name, name);
    strcpy(app->description, description);
    strcpy(app->short_desc, description);
    strcpy(app->version, "1.0.0");
    strcpy(app->exe_path, exe_path);
    app->type = APP_TYPE_EXE;
    app->category = category;
    app->state = APP_STATE_NOT_INSTALLED;
    app->developer_id = dev->id;
    strcpy(app->developer_name, dev->name);
    app->permissions = permissions;
    app->requires_wine = true;
    app->is_free = true;
    app->age_rating = 0;

    dev->total_apps++;
    store.app_count++;

    return true;
}

// ============================================
// Değerlendirme Ekleme
// ============================================
bool store_add_review(uint32_t app_id, const char* username, uint8_t rating, const char* comment) {
    for (uint32_t i = 0; i < store.app_count; i++) {
        if (store.apps[i].id == app_id) {
            store_app_t* app = &store.apps[i];

            if (app->review_count >= MAX_REVIEWS) return false;
            if (rating < 1 || rating > 5) return false;

            app_review_t* review = &app->reviews[app->review_count];
            strcpy(review->username, username);
            review->rating = rating;
            strcpy(review->comment, comment);
            review->helpful = false;

            app->rating_sum += rating;
            app->rating_count++;
            app->avg_rating = (float)app->rating_sum / app->rating_count;

            app->review_count++;
            return true;
        }
    }
    return false;
}

// ============================================
// UI Çizim Fonksiyonları
// ============================================

// Ana mağaza sayfası
void store_draw_main(framebuffer_t* fb) {
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0xF5F5F5);

    // Başlık çubuğu
    fb_fill_rect(fb, 0, 0, fb->width, 60, 0x0078D4);
    // Logo placeholder
    fb_fill_rect(fb, 20, 12, 36, 36, 0xFFFFFF);
    // Başlık
    fb_fill_rect(fb, 70, 18, 200, 24, 0xFFFFFF);
    // Arama kutusu
    fb_draw_glass_panel(fb, 300, 10, 500, 40, 0x60);
    fb_fill_rect(fb, 310, 18, 24, 24, 0x999999); // Search icon
    // Profil
    fb_fill_rect(fb, fb->width - 60, 12, 36, 36, 0xFFFFFF);

    // Sol kategori paneli
    int nav_w = 220;
    fb_draw_glass_panel(fb, 0, 60, nav_w, fb->height - 60, 0x30);

    // Kategori başlığı
    fb_fill_rect(fb, 15, 75, 190, 20, 0x1A1A1A);

    for (int i = 0; i <= 14; i++) {
        int item_y = 105 + i * 34;
        uint32_t bg = (i == store.filter_category) ? 0x0078D4 : 0x00000000;
        fb_fill_rect(fb, 10, item_y, 200, 30, bg);
        // İkon placeholder
        fb_fill_rect(fb, 18, item_y + 7, 16, 16, bg == 0x0078D4 ? 0xFFFFFF : 0x0078D4);
    }

    // Ana içerik alanı
    int content_x = nav_w + 20;
    int content_w = fb->width - nav_w - 40;

    // Öne çıkan uygulamalar başlığı
    fb_fill_rect(fb, content_x, 75, 200, 24, 0x1A1A1A);

    // Öne çıkan uygulama kartları (yatay kaydırma)
    int card_w = 280;
    int card_h = 160;
    int card_y = 110;

    int featured_count = 0;
    for (uint32_t i = 0; i < store.app_count && featured_count < 4; i++) {
        if (store.apps[i].featured || store.apps[i].avg_rating >= 4.5f) {
            int card_x = content_x + featured_count * (card_w + 15);

            // Kart (cam efekti)
            fb_draw_glass_panel(fb, card_x, card_y, card_w, card_h, 0x50);

            // Uygulama ikonu
            fb_fill_rect(fb, card_x + 15, card_y + 15, 48, 48, 0x0078D4);

            // İsim
            fb_fill_rect(fb, card_x + 75, card_y + 15, 180, 16, 0x1A1A1A);

            // Geliştirici
            fb_fill_rect(fb, card_x + 75, card_y + 38, 150, 12, 0x666666);

            // Puan
            fb_fill_rect(fb, card_x + 15, card_y + 75, 60, 14, 0xFFC107);

            // Boyut
            fb_fill_rect(fb, card_x + 85, card_y + 75, 80, 12, 0x999999);

            // Kur/Yükle butonu
            uint32_t btn_color = 0x0078D4;
            fb_fill_rect(fb, card_x + card_w - 80, card_y + card_h - 40, 65, 28, btn_color);

            // Ekran görüntüleri küçük
            for (int s = 0; s < 3; s++) {
                fb_fill_rect(fb, card_x + 15 + s * 60, card_y + 100, 55, 40, 0xCCCCCC);
            }

            featured_count++;
        }
    }

    // Popüler uygulamalar başlığı
    int popular_y = card_y + card_h + 30;
    fb_fill_rect(fb, content_x, popular_y, 200, 24, 0x1A1A1A);
    popular_y += 35;

    // Uygulama listesi (satır halinde)
    store_app_t* filtered[MAX_STORE_APPS];
    int filtered_count = store_filter_apps(filtered, MAX_STORE_APPS);

    for (int i = 0; i < MIN(filtered_count, 12); i++) {
        store_app_t* app = filtered[i];
        int row_y = popular_y + i * 65;

        // Satır arka planı
        fb_draw_glass_panel(fb, content_x, row_y, content_w, 60, 0x30);

        // Uygulama ikonu
        fb_fill_rect(fb, content_x + 10, row_y + 8, 44, 44, 0x0078D4);

        // Uygulama adı
        fb_fill_rect(fb, content_x + 65, row_y + 8, 200, 16, 0x1A1A1A);

        // Geliştirici
        fb_fill_rect(fb, content_x + 65, row_y + 30, 150, 12, 0x666666);

        // Kategori etiketi
        fb_fill_rect(fb, content_x + 250, row_y + 12, 80, 16, 0xE8E8E8);

        // Puan (yıldız)
        fb_fill_rect(fb, content_x + content_w - 200, row_y + 12, 50, 16, 0xFFC107);

        // Boyut
        fb_fill_rect(fb, content_x + content_w - 140, row_y + 12, 80, 12, 0x999999);

        // Kur/Yükle butonu
        uint32_t btn_color = app->state == APP_STATE_INSTALLED ? 0x4CAF50 : 0x0078D4;
        fb_fill_rect(fb, content_x + content_w - 80, row_y + 10, 65, 32, btn_color);
    }

    // Alt durum çubuğu
    fb_fill_rect(fb, 0, fb->height - 32, fb->width, 32, 0xE0E0E0);
    // Depolama bilgisi
    fb_fill_rect(fb, 20, fb->height - 24, 200, 14, 0x666666);
}

// Uygulama detay sayfası
void store_draw_detail(framebuffer_t* fb, store_app_t* app) {
    if (!app) return;

    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0xF5F5F5);

    // Üst banner
    fb_fill_rect(fb, 0, 0, fb->width, 80, 0x0078D4);
    // Geri butonu
    fb_fill_rect(fb, 20, 20, 32, 32, 0xFFFFFF);

    // İkon
    fb_fill_rect(fb, 40, 100, 80, 80, 0x0078D4);

    // Uygulama adı
    fb_fill_rect(fb, 140, 105, 350, 28, 0x1A1A1A);

    // Geliştirici
    fb_fill_rect(fb, 140, 140, 200, 16, 0x0078D4);

    // Puan
    fb_fill_rect(fb, 140, 165, 80, 18, 0xFFC107);

    // Kur/Yükle butonu
    uint32_t btn_color = app->state == APP_STATE_INSTALLED ? 0x4CAF50 : 0x0078D4;
    fb_fill_rect(fb, fb->width - 200, 110, 160, 44, btn_color);

    // İstatistikler
    int stat_y = 200;
    // İndirme
    fb_fill_rect(fb, 40, stat_y, 120, 40, 0xF0F0F0);
    // Boyut
    fb_fill_rect(fb, 180, stat_y, 120, 40, 0xF0F0F0);
    // Yaş sınırı
    fb_fill_rect(fb, 320, stat_y, 120, 40, 0xF0F0F0);
    // Tip
    fb_fill_rect(fb, 460, stat_y, 120, 40, 0xF0F0F0);

    // Ekran görüntüleri
    int ss_y = stat_y + 60;
    fb_fill_rect(fb, 40, ss_y, 200, 20, 0x1A1A1A);
    ss_y += 30;
    for (int i = 0; i < 4; i++) {
        fb_fill_rect(fb, 40 + i * 200, ss_y, 185, 105, 0xCCCCCC);
    }

    // Açıklama
    int desc_y = ss_y + 120;
    fb_fill_rect(fb, 40, desc_y, 200, 20, 0x1A1A1A);
    desc_y += 30;
    fb_fill_rect(fb, 40, desc_y, fb->width - 80, 80, 0xF0F0F0);

    // Güncelleme bilgisi
    int upd_y = desc_y + 100;
    fb_fill_rect(fb, 40, upd_y, 200, 20, 0x1A1A1A);
    upd_y += 30;
    fb_fill_rect(fb, 40, upd_y, fb->width - 80, 40, 0xF0F0F0);

    // İzinler
    int perm_y = upd_y + 60;
    fb_fill_rect(fb, 40, perm_y, 200, 20, 0x1A1A1A);
    perm_y += 30;

    uint32_t perms = app->permissions;
    for (int i = 0; i < 8; i++) {
        if (perms & (1 << i)) {
            fb_fill_rect(fb, 40, perm_y + i * 28, fb->width - 80, 24, 0xF0F0F0);
            fb_fill_rect(fb, 50, perm_y + i * 28 + 4, 16, 16, 0x0078D4);
        }
    }

    // Yorumlar
    int rev_y = perm_y + 8 * 28 + 20;
    fb_fill_rect(fb, 40, rev_y, 200, 20, 0x1A1A1A);
    rev_y += 30;

    for (uint32_t i = 0; i < MIN(app->review_count, 5); i++) {
        fb_fill_rect(fb, 40, rev_y, fb->width - 80, 60, 0xF0F0F0);
        // Kullanıcı adı
        fb_fill_rect(fb, 50, rev_y + 8, 120, 14, 0x1A1A1A);
        // Puan
        fb_fill_rect(fb, 180, rev_y + 8, 60, 14, 0xFFC107);
        // Yorum
        fb_fill_rect(fb, 50, rev_y + 30, fb->width - 120, 20, 0x666666);
        rev_y += 70;
    }

    // Geliştirici Bilgileri
    int dev_y = rev_y + 20;
    fb_fill_rect(fb, 40, dev_y, 200, 20, 0x1A1A1A);
    dev_y += 30;
    fb_fill_rect(fb, 40, dev_y, fb->width - 80, 80, 0xF0F0F0);

    // Geliştirici konsolu butonu (giriş yapılmışsa)
    if (store.current_developer >= 0) {
        fb_fill_rect(fb, 40, dev_y + 100, 200, 36, 0x0078D4);
    }
}

// Yükleme onay diyaloğu
void store_draw_install_dialog(framebuffer_t* fb, store_app_t* app) {
    if (!app) return;

    // Yarı saydam arka plan
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0x000000);

    // Diyalog
    int dlg_w = 500;
    int dlg_h = 400;
    int dlg_x = (fb->width - dlg_w) / 2;
    int dlg_y = (fb->height - dlg_h) / 2;

    fb_draw_glass_panel(fb, dlg_x, dlg_y, dlg_w, dlg_h, 0x95);

    // Başlık
    fb_fill_rect(fb, dlg_x + 30, dlg_y + 20, dlg_w - 60, 28, 0x1A1A1A);

    // Uygulama bilgisi
    fb_fill_rect(fb, dlg_x + 30, dlg_y + 60, 48, 48, 0x0078D4);
    fb_fill_rect(fb, dlg_x + 90, dlg_y + 65, 250, 16, 0x1A1A1A);
    fb_fill_rect(fb, dlg_x + 90, dlg_y + 88, 150, 12, 0x666666);

    // Uygulama boyutu
    fb_fill_rect(fb, dlg_x + 30, dlg_y + 120, dlg_w - 60, 24, 0x666666);

    // İzinler başlığı
    fb_fill_rect(fb, dlg_x + 30, dlg_y + 160, 200, 20, 0x1A1A1A);

    // İzin listesi
    uint32_t perms = app->permissions;
    int perm_y = dlg_y + 190;
    for (int i = 0; i < 8; i++) {
        if (perms & (1 << i)) {
            fb_fill_rect(fb, dlg_x + 40, perm_y, 16, 16, 0xFF9800);
            fb_fill_rect(fb, dlg_x + 65, perm_y + 1, 200, 14, 0x1A1A1A);
            perm_y += 26;
        }
    }

    // Uyarı
    if (app->requires_wine) {
        fb_fill_rect(fb, dlg_x + 30, perm_y + 10, dlg_w - 60, 30, 0xFFF3E0);
        fb_fill_rect(fb, dlg_x + 40, perm_y + 15, 16, 16, 0xFF9800);
        fb_fill_rect(fb, dlg_x + 65, perm_y + 15, 300, 14, 0xE65100);
        perm_y += 50;
    }

    // Butonlar
    // Kur butonu
    fb_fill_rect(fb, dlg_x + dlg_w - 200, dlg_y + dlg_h - 55, 80, 36, 0x0078D4);
    // İptal
    fb_fill_rect(fb, dlg_x + dlg_w - 110, dlg_y + dlg_h - 55, 80, 36, 0x999999);
}

// Geliştirici konsolu
void store_draw_dev_console(framebuffer_t* fb) {
    if (store.current_developer < 0) return;

    developer_account_t* dev = &store.developers[store.current_developer];

    fb_fill_rect(fb, 0, 0, fb->width, fb->height, 0xF5F5F5);

    // Başlık
    fb_fill_rect(fb, 0, 0, fb->width, 60, 0x1A1A2E);
    fb_fill_rect(fb, 20, 15, 300, 24, 0xFFFFFF);

    // Geliştirici bilgileri kartı
    int y = 80;
    fb_draw_glass_panel(fb, 20, y, fb->width - 40, 120, 0x40);
    fb_fill_rect(fb, 40, y + 15, 48, 48, 0x0078D4); // Logo
    fb_fill_rect(fb, 100, y + 15, 200, 18, 0x1A1A1A); // İsim
    fb_fill_rect(fb, 100, y + 40, 250, 14, 0x666666); // Email
    fb_fill_rect(fb, 100, y + 60, 150, 14, 0x666666); // Şirket

    // Doğrulama rozeti
    if (dev->verified) {
        fb_fill_rect(fb, fb->width - 120, y + 15, 80, 24, 0x4CAF50);
    } else {
        fb_fill_rect(fb, fb->width - 140, y + 15, 100, 24, 0xFF9800);
    }

    // İstatistikler
    fb_fill_rect(fb, 40, y + 85, 120, 20, 0x0078D4);
    fb_fill_rect(fb, 180, y + 85, 120, 20, 0x0078D4);
    fb_fill_rect(fb, 320, y + 85, 120, 20, 0x0078D4);

    // Uygulama yayınlama formu
    y = 220;
    fb_fill_rect(fb, 20, y, 200, 24, 0x1A1A1A);
    y += 35;

    // Form alanları
    const char* fields[] = {
        "Uygulama Adı", "Açıklama", "Sürüm",
        "Kategori", "İzinler", ".exe Dosya Yolu",
        "Yaş Sınırı", "Fiyat"
    };

    for (int i = 0; i < 8; i++) {
        fb_draw_glass_panel(fb, 20, y + i * 50, fb->width - 40, 44, 0x30);
        fb_fill_rect(fb, 30, y + i * 50 + 10, 150, 14, 0x1A1A1A);
        fb_fill_rect(fb, 190, y + i * 50 + 10, fb->width - 230, 24, 0xF0F0F0);
    }

    // Yayınla butonu
    fb_fill_rect(fb, fb->width / 2 - 100, y + 8 * 50 + 10, 200, 40, 0x4CAF50);

    // Uygulamalarım
    y += 8 * 50 + 70;
    fb_fill_rect(fb, 20, y, 200, 24, 0x1A1A1A);
    y += 30;

    for (uint32_t i = 0; i < store.app_count; i++) {
        if (store.apps[i].developer_id == dev->id) {
            fb_draw_glass_panel(fb, 20, y, fb->width - 40, 50, 0x30);
            fb_fill_rect(fb, 30, y + 8, 36, 36, 0x0078D4);
            fb_fill_rect(fb, 75, y + 8, 200, 14, 0x1A1A1A);
            fb_fill_rect(fb, 75, y + 28, 150, 12, 0x666666);
            y += 55;
        }
    }
}

// ============================================
// Mağaza durumu erişimcileri
// ============================================
store_state_t* store_get_state(void) {
    return &store;
}

store_app_t* store_get_app_by_id(uint32_t id) {
    for (uint32_t i = 0; i < store.app_count; i++) {
        if (store.apps[i].id == id) return &store.apps[i];
    }
    return NULL;
}

store_app_t* store_get_all_apps(void) {
    return store.apps;
}

uint32_t store_get_app_count(void) {
    return store.app_count;
}
