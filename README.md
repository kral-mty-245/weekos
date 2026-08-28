# WeeK OS 1.0.0 - Liquid Glass Desktop Operating System

> "Real kernel, real desktop, real experience"

WeeK OS, C ile yazılmış gerçek bir çekirdeğe, Liquid Glass efektli modern bir masaüstü ortamına ve Windows 11 benzeri arayüze sahip bir işletim sistemidir.

## ✨ Özellikler

### 🖥️ Masaüstü Ortamı
- **Liquid Glass Efekti**: Mica + Acrylic + Blur efektleri (macOS benzeri)
- **Windows 11 Tarzı Taskbar**: Başlat menüsü, arama, hızlı ayarlar
- **Sanal Masaüstüler**: Sınırsız sanal masaüstü desteği
- **Snap Layouts**: Pencere tutturma ve düzeni
- **Masaüstü İkonları**: Sürükle-bırak desteği
- **Bağlam Menüsü**: Sağ tık menüsü

### ⚙️ Ayarlar Uygulaması
- **Sistem**: Ekran, Ses, Bildirimler, Güç ve Pil, Depolama
- **Bluetooth ve Cihazlar**: Kablosuz cihaz yönetimi
- **Ağ ve İnternet**: WiFi, VPN, Uçak Modu
- **Kişiselleştirme**: Tema, Duvar Kağıdı, Renkler
- **Uygulamalar**: Yüklü uygulama yönetimi
- **Hesaplar**: Kullanıcı profilleri
- **Zaman ve Dil**: Bölgesel ayarlar
- **Erişilebilirlik**: Engelli kullanıcı desteği
- **Gizlilik ve Güvenlik**: WeeK Defender ayarları
- **WeeK OS Update**: Sistem güncellemeleri

### 🛡️ WeeK Defender
- **Gerçek Zamanlı Koruma**: Arka planda sürekli tarama
- **Duvar Güvenliği**: Ağ trafiği filtreleme
- **Akıllı Uygulama Denetimi**: İmzasız uygulamaları engelleme
- **Tarama Modları**: Hızlı, Tam, Özel tarama
- **Tehdit Algılama**: Kırmızı ekran uyarı sistemi
- **Dosya Silme**: Zararlı dosyaları otomatik silme

### 📁 Dosya Gezgini
- **Sekmeler**: Tarayıcı benzeri sekmeli dosya yönetimi
- **Hızlı Erişim**: Sık kullanılan klasörler
- **Arama**: Dosya arama
- **Önizleme**: Dosya önizleme
- **Sürükle-Bırak**: Dosya taşıma/kopyalama

### 📊 Görev Yöneticisi
- **Surecler**: Çalışan uygulamaları listeleme
- **Performans**: CPU, RAM, Disk, Ağ grafikleri
- **Başlangıç**: Başlangıç uygulamaları yönetimi
- **Kullanıcılar**: Aktif kullanıcılar
- **Ayrıntılar**: Detaylı süreç bilgisi
- **Hizmetler**: Sistem hizmetleri

### 💻 Terminal
- **WeeK Terminal (Bash)**: Unix benzeri komut satırı
- **WeeK Shell (PowerShell)**: PowerShell benzeri komut dili
- **Komut Desteği**: ls, cd, pwd, cat, mkdir, rm, cp, mv, echo, neofetch
- **Tab Tamamlama**: Komut otomatik tamamlama
- **Geçmiş**: Komut geçmişi

### 📸 Ekran Alıntısı Aracı
- **Win+Shift+S**: Ekran görüntüsü alma
- **Dikdörtgen Seçim**: Ekranın belirli bir bölgesini yakalama
- **Serbest Seçim**: Serbest 형태 seçim
- **Pencere Yakalama**: Tek pencere yakalama
- **Tam Ekran**: Tüm ekran yakalama

### 📝 Yapışkan Notlar
- **Renkli Notlar**: Sarı, Pembe, Yeşil, Mavi, Mor, Turuncu
- **Sürükle-Bırak**: Masaüstünde taşıma
- **Boyutlandırma**: Özelleştirilebilir boyut
- **Miniaturize**: Küçültme desteği

### 🎨 Hızlı Ayarlar
- **WiFi**: Açma/Kapama
- **Bluetooth**: Açma/Kapama
- **Uçak Modu**: Uçak modu
- **VPN**: VPN bağlantısı
- **Parlaklık**: Ekran parlaklığı ayarı
- **Ses**: Ses seviyesi ayarı
- **Gece Işığı**: Mavi ışık filtresi
- **Pil**: Pil durumu

## 🛠️ Teknik Özellikler

### Çekirdek
- **Dil**: C (GCC ile derlenir)
- **Mimari**: x86 (i386)
- **Bellek Yöneticisi**: PMM + VMM
- **Süreç Zamanlayıcı**: Round-Robin öncelikli
- **IRQ Yöneticisi**: PIC remapping
- **Dosya Sistemi**: VFS + ext2 + RAMDisk
- **Boot**: GRUB Multiboot

### Sürücüler
- **Klavye**: PS/2 QWERTY
- **Fare**: PS/2
- **Disk**: ATA/IDE
- **Ekran**: VGA Framebuffer
- **Ağ**: Ethernet (planlanan)

### Uygulamalar
- **Masaüstü Yöneticisi**: Pencere yönetimi
- **Ayarlar**: Sistem ayarları
- **WeeK Defender**: Antivirüs
- **Görev Yöneticisi**: Süreç yönetimi
- **Dosya Gezgini**: Dosya yönetimi
- **Terminal**: Komut satırı
- **Ekran Alıntısı**: Ekran görüntüsü
- **Yapışkan Notlar**: Not alma

## 🏗️ Derleme

### Gereksinimler
- GCC (i686-elf veya i386-elf)
- NASM
- LD (binutils)
- GRUB (grub-mkrescue)
- xorriso
- mtools

### Derleme
```bash
chmod +x build.sh
./build.sh
```

### ISO Oluşturma
```bash
# Manuel ISO oluşturma
mkdir -p isodir/boot/grub
cp build/kernel.bin isodir/boot/
cp boot/grub.cfg isodir/boot/grub/
grub-mkrescue -o weekos.iso isodir
```

## 🖥️ VirtualBox'ta Çalıştırma

1. VirtualBox'ı açın
2. "Yeni" butonuna tıklayın
3. Ad: "WeeK OS"
4. Tip: Linux -> Other Linux (32-bit)
5. RAM: 512 MB+
6. Sabit disk: Sanal disk oluşturun
7. Ayarlar -> Depolama -> weekos.iso ekleyin
8. Başlat

## 📁 Proje Yapısı

```
weekos/
├── boot/                    # Bootloader ve GRUB
│   ├── boot.asm            # Assembly boot stub
│   └── grub.cfg            # GRUB yapılandırması
├── kernel/                  # Çekirdek kodu
│   ├── main.c             # Ana giriş noktası
│   ├── io.c               # I/O fonksiyonları
│   ├── string.c           # String fonksiyonları
│   ├── printf.c           # Printf uygulaması
│   ├── mm/                # Bellek yönetimi
│   │   ├── pmm.c          # Fiziksel bellek yöneticisi
│   │   ├── vmm.c          # Sanal bellek yöneticisi
│   │   └── heap.c         # Heap yöneticisi
│   ├── sched/             # Süreç zamanlayıcı
│   │   ├── scheduler.c    # Zamanlayıcı
│   │   └── process.c      # Süreç yönetimi
│   ├── irq/               # Kesme yönetimi
│   │   ├── irq.c          # IRQ handler
│   │   └── timer.c        # Zamanlayıcı kesmesi
│   └── fs/                # Dosya sistemi
│       ├── vfs.c          # Sanal dosya sistemi
│       ├── ext2.c         # ext2 sürücüsü
│       └── ramdisk.c      # RAM disk
├── drivers/                 # Sürücüler
│   ├── vga/               # Ekran sürücüsü
│   │   ├── vga.c          # VGA text mode
│   │   └── framebuffer.c  # Framebuffer + glass efektleri
│   ├── keyboard/          # Klavye sürücüsü
│   │   └── keyboard.c     # PS/2 klavye
│   ├── mouse/             # Fare sürücüsü
│   │   └── mouse.c        # PS/2 fare
│   └── disk/              # Disk sürücüleri
│       ├── ata.c          # ATA/IDE
│       └── ide.c          # IDE kontrolcü
├── userspace/               # Kullanıcı alanı uygulamaları
│   ├── gui/               # Grafik arayüz
│   │   ├── desktop/       # Masaüstü ortamı
│   │   │   └── desktop.c  # Liquid glass masaüstü
│   │   ├── settings/      # Ayarlar uygulaması
│   │   │   ├── settings.c # Ana ayarlar
│   │   │   └── quicksettings.c # Hızlı ayarlar
│   │   ├── defender/      # WeeK Defender
│   │   │   └── defender.c # Antivirüs
│   │   ├── taskmanager/   # Görev yöneticisi
│   │   │   └── taskmanager.c
│   │   ├── explorer/      # Dosya gezgini
│   │   │   └── explorer.c
│   │   ├── snipping/      # Ekran alıntısı
│   │   │   └── snipping.c
│   │   └── sticky/        # Yapışkan notlar
│   │       └── sticky.c
│   └── apps/              # Uygulamalar
│       └── terminal/      # Terminal
│           └── terminal.c
├── installer/               # Kurulum sihirbazı
│   └── installer.c        # Windows 11 benzeri kurulum
├── include/                 # Başlık dosyaları
│   ├── types.h            # Temel tipler
│   ├── kernel.h           # Ana kernel başlığı
│   ├── stdarg.h           # Değişken argümanlar
│   └── registers.h        # Kayıt yapısı
├── Makefile                # Derleme sistemi
├── linker.ld               # Linker betiği
├── build.sh                # Derleme betiği
└── README.md               # Bu dosya
```

## 🎨 Tema Özelleştirmesi

### Varsayılan Tema (Açık)
- Arka Plan: Beyaz (#FFFFFF)
- Vurgu: Mavi (#0078D4)
- Metin: Koyu (#1A1A1A)

### Karanlık Tema
- Arka Plan: Koyu (#2D2D2D)
- Vurgu: Mavi (#0078D4)
- Metin: Açık (#FFFFFF)

### Liquid Glass Efekti
- Mica: Yarı saydam arka plan
- Acrylic: Bulanıklaştırma efekti
- Blur: Arka plan bulanıklığı
- Opacity: %40-%90 ayarlanabilir

## 🔐 Güvenlik

### WeeK Defender
- Gerçek zamanlı koruma
- Duvar güvenliği
- Akıllı uygulama denetimi
- Tarama motoru
- Tehdit algılama

### Sistem Güvenliği
- Kullanıcı izinleri
- Dosya izinleri (rwx)
- Bellek koruma
- Süreç izolasyonu

## 📝 Lisans

WeeK OS, özgür ve açık kaynaklı bir projedir.

## 👨‍💻 Geliştirici

WeeK OS Development Team

---

**WeeK OS** - Liquid Glass Desktop Experience 🌊💎
