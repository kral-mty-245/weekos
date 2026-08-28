#!/bin/bash
# WeeK OS - Docker Build Script
# Bu betik Docker kullanarak ISO oluşturur

set -e

echo "============================================"
echo "  WeeK OS Docker Build System"
echo "============================================"
echo ""

# Docker var mı kontrol et
if ! command -v docker &> /dev/null; then
    echo "❌ Docker bulunamadı!"
    echo ""
    echo "Docker'ı kurun:"
    echo "  Windows: https://docs.docker.com/desktop/install/windows-install/"
    echo "  Linux:   sudo apt install docker.io"
    echo "  Mac:     https://docs.docker.com/desktop/install/mac-install/"
    exit 1
fi

# Docker Compose var mı kontrol et
if command -v docker-compose &> /dev/null; then
    COMPOSE="docker-compose"
elif docker compose version &> /dev/null; then
    COMPOSE="docker compose"
else
    echo "⚠️  Docker Compose bulunamadı, doğrudan Docker kullanılıyor..."
    COMPOSE=""
fi

echo "[1/4] Docker imajı oluşturuluyor..."
if [ -n "$COMPOSE" ]; then
    $COMPOSE build weekos-builder
else
    docker build -t weekos-builder .
fi

echo ""
echo "[2/4] ISO derleniyor..."
if [ -n "$COMPOSE" ]; then
    $COMPOSE run --rm weekos-builder
else
    docker run --rm -v "$(pwd):/workspace" weekos-builder ./build.sh
fi

echo ""
echo "[3/4] Sonuçlar kontrol ediliyor..."
if [ -f weekos.iso ]; then
    echo "✅ ISO başarıyla oluşturuldu!"
    echo "   Boyut: $(du -h weekos.iso | cut -f1)"
    echo "   Yol: $(pwd)/weekos.iso"
else
    echo "❌ ISO oluşturulamadı!"
    exit 1
fi

echo ""
echo "[4/4] Tamamlandı!"
echo ""
echo "============================================"
echo "  WeeK OS ISO Hazır!"
echo "============================================"
echo ""
echo "VirtualBox'ta çalıştırmak için:"
echo "  1. VirtualBox'ı açın"
echo "  2. Yeni > Linux > Other Linux (32-bit)"
echo "  3. RAM: 512MB+"
echo "  4. weekos.iso'yu CD-ROM'a ekleyin"
echo "  5. Başlat"
echo ""
echo "QEMU ile test etmek için:"
echo "  qemu-system-i386 -cdrom weekos.iso -m 512M -boot d"
echo ""
