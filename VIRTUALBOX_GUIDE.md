# WeeK OS - VirtualBox Test Rehberi & Sistem Gereksinimleri

---

## 🖥️ Sistem Gereksinimleri

### Minimum Gereksinimler (WeeK OS'i çalıştırmak için)

| Bileşen         | Minimum                          | Önerilen                         |
|-----------------|----------------------------------|----------------------------------|
| **İşlemci**     | 1 GHz tek çekirdek (x86)        | 2 GHz çift çekirdek (x86)       |
| **Bellek (RAM)**| 512 MB                           | 2 GB                             |
| **Depolama**    | 5 GB boş alan                    | 20 GB boş alan                   |
| **Ekran**       | 800x600                          | 1920x1080 (Full HD)             |
| **İnternet**    | Gerekli değil (updates için var) | WiFi veya Ethernet               |
| **Optik Sürücü**| USB flash veya .iso dosyası      | VirtualBox/VMware               |

### Donanım Desteği

| Bileşen         | Durum     | Not                              |
|-----------------|-----------|----------------------------------|
| **CPU**         | ✅ Tam    | Intel/AMD x86 (32-bit)          |
| **RAM**         | ✅ Tam    | 512MB - 16GB arası              |
| **Disk**        | ✅ Tam    | ATA/IDE, SATA, NVMe (planlanan) |
| **Ekran**       | ✅ Tam    | VGA, VESA, Framebuffer          |
| **Klavye**      | ✅ Tam    | PS/2 QWERTY                     |
| **Fare**        | ✅ Tam    | PS/2 3-butonlu                   |
| **WiFi**        | ✅ Tam    | RTL8188, RTL8821, Atheros       |
| **Bluetooth**   | ✅ Tam    | Bluetooth 5.3                   |
| **Ses**         | ⚠️ Kısmi | HD Audio (planlanan)            |
| **USB**         | ⚠️ Kısmi | USB 2.0 (planlanan)             |
| **Ethernet**    | ⚠️ Kısmi | Intel E1000, Realtek RTL8139    |
| **GPU**         | ⚠️ Kısmi | VGA/VESA, 3D (planlanan)        |

### Yazılım Gereksinimleri

| Yazılım         | Sürüm      | Amaç                              |
|-----------------|------------|-----------------------------------|
| **VirtualBox**  | 7.0+       | Sanal makine                      |
| **QEMU**        | 8.0+       | Alternatif sanal makine           |
| **Docker**      | 24.0+      | Build ortamı (opsiyonel)          |
| **GCC**         | 12.0+      | Derleme (Linux'ta)                |
| **NASM**        | 2.16+      | Assembly derleyici                |
| **GRUB**        | 2.06+      | Bootloader                        |

---

## 📦 VirtualBox'ta Test Adımları

### Adım 1: WeeK OS ISO'sunu Oluştur

#### Yöntem A: Linux ile Derleme
```bash
# Linux terminalinde (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential nasm grub-pc-bin grub-common xorriso mtools

cd weekos/
chmod +x build.sh
./build.sh
```

#### Yöntem B: Docker ile Derleme
```bash
# Docker yüklü herhangi bir sistemde
cd weekos/
chmod +x build-docker.sh
./build-docker.sh
```

#### Yöntem C: Hazır ISO (İndirme)
```bash
# Eğer ISO hazırsa doğrudan VirtualBox'a geçin
# weekos.iso dosyasının konumunu not edin
```

---

### Adım 2: VirtualBox'ta Yeni Sanal Makine Oluşturma

1. **VirtualBox'ı açın**
2. **"Yeni" (New) butonuna tıklayın**

3. **Adım - Isim ve İşletim Sistemi:**
   ```
   Name:     WeeK OS 1.0.0
   Type:     Linux
   Version:  Other Linux (32-bit)
   ```
   > ⚠️ "Version" kısmında "Other Linux (32-bit)" seçin!
   > WeeK OS 32-bit x86 çekirdeği kullanır.

4. **Adım - Bellek Boyutu:**
   ```
   RAM: 1024 MB (1 GB) minimum
   Önerilen: 2048 MB (2 GB)
   Maksimum: 4096 MB (4 GB)
   ```
   > 💡 512MB ile de çalışır ama 1GB+ önerilir.

5. **Adım - Sanal Disk:**
   ```
   Disk tipi:    Sanal hard disk (VDI)
   Depolama:     Dynamically allocated
   Boyut:        20 GB (minimum 5 GB yeterli)
   ```

6. **Bitir** butonuna tıklayın.

---

### Adım 3: Sanal Makine Ayarları

Sanal makineyi seçin ve **"Ayarlar" (Settings)** butonuna tıklayın.

#### 🖥️ Sistem (System)
```
Motherboard:
  Base Memory:     2048 MB
  Boot Order:      Optical, Hard Disk
  Chipset:         PIIX3
  TPM:            _NONE
  EFI:             Kapalı (WeeK OS GRUB kullanır)

Processor:
  Processors:      2 (çift çekirdek)
  Execution Cap:   100%
  Enable PAE/NX:   Kapalı

Acceleration:
  Paravirtualization: Default
  Virtualization:     Enable VT-x/AMD-V
  Nested Paging:      Enable
```

#### 🖼️ Ekran (Display)
```
Video:
  Video Memory:    128 MB
  Monitor Count:   1
  Scale Factor:    100%
  Graphics Controller: VMSVGA veya VBoxSVGA
  
Remote Display:   Kapalı
```

#### 💾 Depolama (Storage)
```
Storage Devices:
  Controller: IDE
    IDE Secondary: weekos.iso (boş CD-ROM'a bağlayın)
    
  Controller: SATA
    SATA Port 0: WeeK-OS.vdi (20GB sanal disk)
```

> 📌 **ISO'yu Bağlama:**
> 1. IDE Secondary'yi seçin
> 2. Sağ taraftaki disk simgesine tıklayın
> 3. "Choose a disk file..." seçin
> 4. `weekos.iso` dosyasını seçin

#### 🌐 Ağ (Network)
```
Adapter 1:
  Enable Network Adapter:  ✓
  Attached to:             NAT
  Adapter Type:            Intel PRO/1000 MT Desktop
  Cable Connected:         ✓

  Advanced:
    Promiscuous Mode:      Deny
    MAC Address:           (otomatik)
    Enable VLAN:           Kapalı
```

#### 🔊 Ses (Audio)
```
Audio:
  Enable Audio:    ✓
  Host Driver:     Windows DirectSound / Linux ALSA
  Controller:      Intel HD Audio
```

#### 📀 USB
```
USB:
  Enable USB:      ✓
  USB Controller:  USB 2.0 (EHCI)
```

---

### Adım 4: Sanal Makineyi Başlatma

1. **"Başlat" (Start) butonuna tıklayın**
2. **GRUB ekranı görünecek:**
   ```
   ┌─────────────────────────────────────┐
   │  WeeK OS 1.0.0                     │
   │  WeeK OS 1.0.0 (Safe Mode)        │
   │  WeeK OS 1.0.0 (Recovery)         │
   │  Memory Test (memtest86+)          │
   └─────────────────────────────────────┘
   ```
3. **"WeeK OS 1.0.0" seçin ve Enter'a basın**

4. **Boot ekranı görünecek:**
   ```
     __        __   _    _____                    _             
     \ \      / /__| |__|_   _|__ _ __ _ __ ___ (_)_ __   __ _ 
      \ \ /\ / / _ \ '_ \ | |/ _ \ '__| '_ ` _ \| | '_ \ / _` |
       \ V  V /  __/ |_) || |  __/ |  | | | | | | | | | | (_| |
        \_/\_/ \___|_.__/ |_|\___|_|  |_| |_| |_|_|_| |_|\__, |
                                                           |___/ 
   ```
5. **Kurulum sihirbazı başlayacak**

---

### Adım 5: Kurulum Sihirbazı

Kurulum adımları:

1. **Dil Seçimi**
   - Türkçe'yi seçin → İleri

2. **Bölge Seçimi**
   - Türkiye'yi seçin → İleri

3. **Klavye Düzeni**
   - Türkçe Q'yu seçin → İleri

4. **Lisans Anlaşması**
   - Kabul ediyorum onay kutusunu işaretleyin → İleri

5. **Kurulum Türü**
   - "Özel (Gelişmiş)" seçin → İleri

6. **Disk Seçimi**
   - 20GB sanal diski seçin → İleri

7. **Hesap Oluşturma**
   - Kullanıcı adı: (örn: kerem)
   - Şifre: (bir şifre girin)
   - Bilgisayar adı: WeeK-PC
   → İleri

8. **Kişiselleştirme**
   - Tema: Açık / Karanlık / Otomatik
   - Vurgu rengi: Mavi
   → Kur

9. **Kurulum Başlıyor**
   ```
   %0 ━━━━━━━━━━━━━━━━━━━━━━━━━━━━ %100
   ```
   ≈ 2-5 dakika

10. **Kurulum Tamamlandı**
    - "Yeniden Başlat" butonuna tıklayın
    - ISO'yu çıkarmayı unutmayın!

---

### Adım 6: İlk Kullanıcı Deneyimi

Kurulumdan sonra masaüstü görünecek:

1. **Taskbar (Görev Çubuğu)**
   - Sol: Başlat menüsü
   - Orta: Arama, sabit uygulamalar
   - Sağ: WiFi, Bluetooth, Ses, Pil, Saat

2. **Masaüstü**
   - Sol üst: Bilgisayar, Geri Dönüşüm Kutusu, WeeK Store ikonları
   - Arka plan: Liquid Glass efektli duvar kağıdı

3. **Ayarlar**
   - Başlat → Ayarlar
   - Tüm ayarlar mevcut: Sistem, Bluetooth, Ağ, Kişiselleştirme, vb.

4. **Terminal**
   - Başlat → Terminal veya Ctrl+Alt+T

5. **Görev Yöneticisi**
   - Ctrl+Shift+Esc

6. **WeeK Store**
   - Masaüstü ikonu veya Başlat menüsü

---

## 🔧 Sık Karşılaşılan Sorunlar

### Sorun 1: "No bootable medium found"
**Çözüm:** ISO dosyasının doğru bağlandığından emin olun
- Ayarlar → Depolama → IDE Secondary → weekos.iso

### Sorun 2: "Kernel panic"
**Çözüm:**
- RAM'i artırın (en az 512MB)
- Safe Mode'u deneyin
- CPU çekirdeğini 1'e düşürün

### Sorun 3: "Ekran çok küçük"
**Çözüm:**
- VirtualBox Guest Tools kurun (sonraki aşama)
- Ekran çözünürlüğünü değiştirin:
  - Görünüm → Ekran Çözünürlüğü → 1920x1080

### Sorun 4: "Mouse çalışmıyor"
**Çözüm:**
- Fare otomatik yakalama aktif
- Sol Ctrl tuşu ile serbest bırakın
- USB fare bağlamayı deneyin

### Sorun 5: "Ses yok"
**Çözüm:**
- Ses ayarlarını kontrol edin
- Alternatif sürücü deneyin (ALSA/DirectSound)

### Sorun 6: "İnternet yok"
**Çözüm:**
- Ağ ayarlarını kontrol edin
- NAT modunu deneyin
- Bridged adapter'a geçin

---

## 📊 Performans İpuçları

### VirtualBox Optimizasyonları
```
1. VT-x/AMD-V aktif edin (BIOS'tan)
2. Nested Paging aktif edin
3. 2+ CPU çekirdeği verin
4. 128MB video bellek verin
5. VMSVGA graphics controller kullanın
6. Aero Glass efektleri için 3D Acceleration deneyin
```

### QEMU ile Hızlı Test
```bash
# QEMU ile hızlı test (GUI yok)
qemu-system-i386 -cdrom weekos.iso -m 1024M -vga std -boot d

# KVM hızlandırma ile (Linux)
qemu-system-i386 -cdrom weekos.iso -m 1024M -vga std -boot d -enable-kvm

# Ağ desteği ile
qemu-system-i386 -cdrom weekos.iso -m 1024M -vga std -boot d -net nic -net user
```

### VMware ile Test
```
1. Yeni sanal makine oluşturun
2. "Other" → "Other" seçin
3. 2GB RAM, 20GB disk verin
4. weekos.iso'yu bağlayın
5. Başlatın
```

---

## 🧪 Test Senaryoları

### Temel Testler
| # | Test                           | Beklenen Sonuç                    |
|---|--------------------------------|-----------------------------------|
| 1 | Boot                           | GRUB ekranı → Masaüstü gelir      |
| 2 | Kurulum                        | Adım adım kurulum tamamlanır      |
| 3 | Masaüstü                       | Liquid Glass arayüzü görünür      |
| 4 | Başlat menüsü                  | Tüm uygulamalar listelenir       |
| 5 | Ayarlar                        | Tüm kategoriler açılır           |
| 6 | Terminal                       | Komutlar çalışır                  |
| 7 | Dosya Gezgini                  | Klasörler listelenir             |
| 8 | Görev Yöneticisi              | Süreçler gösterilir              |
| 9 | Hızlı Ayarlar                  | WiFi, Bluetooth, ses ayarları     |
|10 | WeeK Store                     | Uygulamalar listelenir           |

### İleri Seviye Testler
| # | Test                           | Beklenen Sonuç                    |
|---|--------------------------------|-----------------------------------|
|11 | Tema değiştirme                | Açık/Karanlık geçişi çalışır     |
|12 | Duvar kağıdı değiştirme        | Duvar kağıdı değişir             |
|13 | Sanal masaüstü                 | Yeni masaüstü eklenir            |
|14 | Pencere yönetimi               | Sürükle, küçült, büyüt           |
|15 | WeeK Defender                  | Tarama çalışır                    |
|16 | Ekran Alıntısı                 | Ekran görüntüsü alınır           |
|17 | Yapışkan Notlar                | Not eklenir/düzenlenir           |
|18 | WiFi tarama                    | Ağlar listelenir                 |
|19 | Bluetooth tarama               | Cihazlar listelenir              |
|20 | WINE (.exe)                     | Windows uygulaması çalışır       |

---

## 📋 WeeK OS Özellik Listesi

### Çekirdek
- [x] C ile yazılmış gerçek x86 kernel
- [x] GRUB Multiboot ile boot
- [x] Fiziksel bellek yöneticisi (PMM)
- [x] Sanal bellek yöneticisi (VMM)
- [x] Heap yöneticisi
- [x] Süreç zamanlayıcı (Round-Robin)
- [x] IRQ yöneticisi (PIC remapping)
- [x] VFS + ext2 dosya sistemi
- [x] RAM disk desteği

### Sürücüler
- [x] VGA text mode
- [x] VGA framebuffer (1920x1080)
- [x] PS/2 klavye
- [x] PS/2 fare
- [x] ATA/IDE disk
- [x] WiFi (RTL8188/RTL8821)
- [x] Bluetooth 5.3
- [ ] USB (planlanan)
- [ ] Ses (planlanan)

### Masaüstü Ortamı
- [x] Liquid Glass efekti (Mica + Acrylic + Blur)
- [x] Windows 11 tarzı taskbar
- [x] Başlat menüsü
- [x] Hızlı Ayarlar paneli
- [x] Bildirim merkezi
- [x] Sanal masaüstü desteği
- [x] Pencere yönetimi
- [x] Masaüstü ikonları
- [x] Bağlam menüsü

### Uygulamalar
- [x] Ayarlar (12 kategori)
- [x] WeeK Defender (antivirüs)
- [x] Görev Yöneticisi
- [x] Dosya Gezgini (sekmeli)
- [x] Terminal (Bash + PowerShell)
- [x] Ekran Alıntısı Aracı
- [x] Yapışkan Notlar
- [x] WeeK Store (mağaza)
- [x] WINE entegrasyonu
- [ ] Hesap Makinesi (planlanan)
- [ ] Paint (planlanan)
- [ ] Medya Oynatıcı (planlanan)

### Güvenlik
- [x] WeeK Defender (antivirüs)
- [x] Gerçek zamanlı koruma
- [x] Tarama motoru
- [x] Tehdit algılama (kırmızı ekran)
- [x] Dosya silme
- [x] Duvar güvenliği
- [x] Akıllı uygulama denetimi

### Kurulum
- [x] Windows 11 tarzı kurulum sihirbazı
- [x] Dil seçimi
- [x] Bölge seçimi
- [x] Klavye düzeni
- [x] Lisans anlaşması
- [x] Disk partitioning
- [x] Hesap oluşturma
- [x] Kişiselleştirme
- [x] Otomatik kurulum

### Derleme
- [x] GCC/NASM/LD derleme zinciri
- [x] GRUB ile ISO oluşturma
- [x] Docker build desteği
- [x] QEMU ile test
- [x] VirtualBox uyumluluğu

---

## 🔗 Faydalı Linkler

- **VirtualBox İndirme:** https://www.virtualbox.org/wiki/Downloads
- **QEMU İndirme:** https://www.qemu.org/download/
- **Docker İndirme:** https://docs.docker.com/get-docker/
- **GRUB Dökümantasyon:** https://www.gnu.org/software/grub/manual/

---

**WeeK OS** - Liquid Glass Desktop Experience 🌊💎
