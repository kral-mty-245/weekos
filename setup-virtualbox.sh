#!/bin/bash
# WeeK OS - VirtualBox Otomatik Kurulum
# Bu betik VirtualBox'ta WeeK OS sanal makinesini otomatik oluşturur

set -e

echo "=========================================="
echo "  WeeK OS VirtualBox Setup"
echo "=========================================="
echo ""

# VirtualBox kontrolü
if ! command -v VBoxManage &> /dev/null; then
    echo "❌ VirtualBox bulunamadı!"
    echo ""
    echo "VirtualBox'ı indirin:"
    echo "  https://www.virtualbox.org/wiki/Downloads"
    exit 1
fi

# ISO kontrolü
if [ ! -f weekos.iso ]; then
    echo "❌ weekos.iso bulunamadı!"
    echo ""
    echo "Önce ISO'yu oluşturun:"
    echo "  chmod +x build.sh && ./build.sh"
    echo ""
    echo "veya Docker ile:"
    echo "  chmod +x build-docker.sh && ./build-docker.sh"
    exit 1
fi

ISO_PATH=$(pwd)/weekos.iso
VM_NAME="WeeK OS"
VM_RAM="2048"
VM_CPU="2"
VM_DISK="20480"

echo "[1/6] Sanal makine oluşturuluyor: $VM_NAME"

# Eski makineyi sil (varsa)
VBoxManage unregistervm "$VM_NAME" --delete 2>/dev/null || true

# Yeni sanal makine
VBoxManage createvm \
    --name "$VM_NAME" \
    --ostype "Linux26_64" \
    --register \
    --basefolder "$(pwd)"

echo "  ✅ Sanal makine oluşturuldu"
echo ""

echo "[2/6] Yapılandırma ayarlanıyor..."

# Bellek ve CPU
VBoxManage modifyvm "$VM_NAME" \
    --memory $VM_RAM \
    --cpus $VM_CPU \
    --vram 128 \
    --graphicscontroller vmsvga \
    --boot1 dvd \
    --boot2 disk \
    --boot3 none \
    --boot4 none \
    --firmware efi \
    --tpm-type 2.0 \
    --paravirtprovider default

# Ağ
VBoxManage modifyvm "$VM_NAME" \
    --nic1 nat \
    --nictype1 82540EM \
    --cableconnected1 on

# Ses
VBoxManage modifyvm "$VM_NAME" \
    --audio enabled \
    --audiocontroller hda \
    --audiocodec alsa

# USB
VBoxManage modifyvm "$VM_NAME" \
    --usb on \
    --usbehci on

echo "  ✅ Yapılandırma tamamlandı"
echo ""

echo "[3/6] Sanal disk oluşturuluyor..."

# SATA kontrolcüsü
VBoxManage storagectl "$VM_NAME" \
    --name "SATA" \
    --add sata \
    --controller IntelAhci

# Sanal disk
VBoxManage createmedium disk \
    --filename "$(pwd)/$VM_NAME/$VM_NAME.vdi" \
    --size $VM_DISK \
    --format VDI

# Disk'i SATA'ya bağla
VBoxManage storageattach "$VM_NAME" \
    --storagectl "SATA" \
    --port 0 \
    --device 0 \
    --type hdd \
    --medium "$(pwd)/$VM_NAME/$VM_NAME.vdi"

echo "  ✅ Sanal disk oluşturuldu ($((VM_DISK/1024))GB)"
echo ""

echo "[4/6] ISO bağlanıyor..."

# IDE kontrolcüsü
VBoxManage storagectl "$VM_NAME" \
    --name "IDE" \
    --add ide

# ISO'yu IDE'ye bağla
VBoxManage storageattach "$VM_NAME" \
    --storagectl "IDE" \
    --port 0 \
    --device 0 \
    --type dvddrive \
    --medium "$ISO_PATH"

echo "  ✅ ISO bağlandı: $ISO_PATH"
echo ""

echo "[5/6] Ek ayarlar..."

# Ek ayarlar
VBoxManage modifyvm "$VM_NAME" \
    --clipboard-mode bidirectional \
    --draganddrop bidirectional \
    --sharedfolders enabled \
    --guestmemoryballoon on \
    --nestedpaging on \
    --largepages on \
    --vtxvpid on \
    --nested-hw-virt on

echo "  ✅ Ek ayarlar tamamlandı"
echo ""

echo "[6/6] Sanal makine başlatılıyor..."

# Sanal makineyi başlat
VBoxManage startvm "$VM_NAME" --type gui

echo ""
echo "=========================================="
echo "  WeeK OS VirtualBox Kurulum Tamamlandı!"
echo "=========================================="
echo ""
echo "  Sanal Makine: $VM_NAME"
echo "  RAM: ${VM_RAM}MB"
echo "  CPU: $VM_CPU çekirdek"
echo "  Disk: $((VM_DISK/1024))GB"
echo "  ISO: $ISO_PATH"
echo ""
echo "  WeeK OS şimdi başlıyor!"
echo "  Kurulum sihirbazı otomatik olarak başlayacak."
echo ""
echo "  İpuçları:"
echo "    - Sol Ctrl ile fareyi serbest bırakın"
echo "    - Tam ekran için: View → Full Screen"
echo "    - Ekranı büyütmek için: View → Auto-resize Guest"
echo ""
