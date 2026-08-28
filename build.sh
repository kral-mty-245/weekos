#!/bin/bash
# WeeK OS Build Script v2.0
# Includes: Kernel, Drivers, GUI, Store, WINE, WiFi, Bluetooth

set -e

echo "=========================================="
echo "  WeeK OS Build System v2.0"
echo "  Building Liquid Glass Desktop OS"
echo "=========================================="
echo ""

# Build tools check
echo "[1/10] Checking build tools..."
if ! command -v gcc &> /dev/null; then
    echo "ERROR: gcc not found"
    echo "Install: sudo apt install build-essential nasm grub-pc-bin grub-common xorriso mtools"
    exit 1
fi

if ! command -v nasm &> /dev/null; then
    echo "ERROR: nasm not found"
    exit 1
fi

if ! command -v ld &> /dev/null; then
    echo "ERROR: ld not found"
    exit 1
fi

if ! command -v grub-mkrescue &> /dev/null; then
    echo "ERROR: grub-mkrescue not found"
    echo "Install: sudo apt install grub-pc-bin grub-common xorriso mtools"
    exit 1
fi
echo "  All tools found!"
echo ""

# Clean
echo "[2/10] Cleaning..."
rm -rf build isodir weekos.iso
mkdir -p build isodir/boot/grub
echo "  Done!"
echo ""

# Assembly
echo "[3/10] Compiling assembly..."
nasm -f elf32 boot/boot.asm -o build/boot.o
nasm -f elf32 kernel/entry.asm -o build/entry.o 2>/dev/null || true
echo "  Done!"
echo ""

# CFLAGS
CFLAGS="-ffreestanding -O2 -Wall -Wextra -Iinclude -m32 -fno-pie -fno-stack-protector -fno-builtin"

# Kernel
echo "[4/10] Compiling kernel..."
gcc $CFLAGS -c kernel/main.c -o build/main.o
gcc $CFLAGS -c kernel/io.c -o build/io.o
gcc $CFLAGS -c kernel/string.c -o build/string.o
gcc $CFLAGS -c kernel/printf.c -o build/printf.o
gcc $CFLAGS -c kernel/mm/pmm.c -o build/pmm.o
gcc $CFLAGS -c kernel/mm/vmm.c -o build/vmm.o
gcc $CFLAGS -c kernel/mm/heap.c -o build/heap.o
gcc $CFLAGS -c kernel/sched/scheduler.c -o build/scheduler.o
gcc $CFLAGS -c kernel/sched/process.c -o build/process.o
gcc $CFLAGS -c kernel/irq/irq.c -o build/irq.o
gcc $CFLAGS -c kernel/irq/timer.c -o build/timer.o
gcc $CFLAGS -c kernel/fs/vfs.c -o build/vfs.o
gcc $CFLAGS -c kernel/fs/ext2.c -o build/ext2.o
gcc $CFLAGS -c kernel/fs/ramdisk.c -o build/ramdisk.o
echo "  Done!"
echo ""

# Drivers
echo "[5/10] Compiling drivers..."
gcc $CFLAGS -c drivers/vga/vga.c -o build/vga.o
gcc $CFLAGS -c drivers/vga/framebuffer.c -o build/framebuffer.o
gcc $CFLAGS -c drivers/keyboard/keyboard.c -o build/keyboard.o
gcc $CFLAGS -c drivers/mouse/mouse.c -o build/mouse.o
gcc $CFLAGS -c drivers/disk/ata.c -o build/ata.o
gcc $CFLAGS -c drivers/disk/ide.c -o build/ide.o
gcc $CFLAGS -c drivers/net/wifi.c -o build/wifi.o
gcc $CFLAGS -c drivers/net/bluetooth.c -o build/bluetooth.o
echo "  Done!"
echo ""

# GUI
echo "[6/10] Compiling GUI applications..."
gcc $CFLAGS -c userspace/gui/desktop/desktop.c -o build/desktop.o
gcc $CFLAGS -c userspace/gui/settings/settings.c -o build/settings.o
gcc $CFLAGS -c userspace/gui/settings/quicksettings.c -o build/quicksettings.o
gcc $CFLAGS -c userspace/gui/defender/defender.c -o build/defender.o
gcc $CFLAGS -c userspace/gui/taskmanager/taskmanager.c -o build/taskmanager.o
gcc $CFLAGS -c userspace/gui/explorer/explorer.c -o build/explorer.o
gcc $CFLAGS -c userspace/gui/snipping/snipping.c -o build/snipping.o
gcc $CFLAGS -c userspace/gui/sticky/sticky.c -o build/sticky.o
gcc $CFLAGS -c userspace/gui/store/store.c -o build/store.o
echo "  Done!"
echo ""

# Apps
echo "[7/10] Compiling applications..."
gcc $CFLAGS -c userspace/apps/terminal/terminal.c -o build/terminal.o
gcc $CFLAGS -c userspace/apps/wine/wine.c -o build/wine.o
gcc $CFLAGS -c installer/installer.c -o build/installer.o
echo "  Done!"
echo ""

# Link
echo "[8/10] Linking kernel..."
OBJS=$(ls build/*.o | tr '\n' ' ')
ld -m elf_i386 -T linker.ld -o build/kernel.bin $OBJS
echo "  Done!"
echo ""

# GRUB
echo "[9/10] Creating bootable image..."
cp build/kernel.bin isodir/boot/
cp boot/grub.cfg isodir/boot/grub/
dd if=/dev/zero of=isodir/boot/initrd.img bs=1M count=1 2>/dev/null
echo "  Done!"
echo ""

# ISO
echo "[10/10] Creating ISO..."
grub-mkrescue -o weekos.iso isodir 2>/dev/null

if [ -f weekos.iso ]; then
    echo ""
    echo "=========================================="
    echo "  Build Successful!"
    echo "=========================================="
    echo ""
    ISO_SIZE=$(du -h weekos.iso | cut -f1)
    echo "  ISO: weekos.iso ($ISO_SIZE)"
    echo ""
    echo "  Features included:"
    echo "    - WeeK OS Kernel (C, x86)"
    echo "    - Liquid Glass Desktop"
    echo "    - Settings (all categories)"
    echo "    - WeeK Defender (antivirus)"
    echo "    - Task Manager"
    echo "    - File Explorer (tabs)"
    echo "    - Terminal (Bash + PowerShell)"
    echo "    - WeeK Store (app marketplace)"
    echo "    - Snipping Tool"
    echo "    - Sticky Notes"
    echo "    - WINE integration (.exe support)"
    echo "    - WiFi driver"
    echo "    - Bluetooth driver"
    echo "    - Installation wizard"
    echo ""
    echo "  Run in VirtualBox:"
    echo "    New VM > Linux > Other Linux (32-bit)"
    echo "    RAM: 512MB+"
    echo "    Attach weekos.iso as CD-ROM"
    echo ""
    echo "  Run in QEMU:"
    echo "    qemu-system-i386 -cdrom weekos.iso -m 512M -boot d"
    echo ""
else
    echo "ERROR: ISO creation failed!"
    exit 1
fi
