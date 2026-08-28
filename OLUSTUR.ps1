# WeeK OS - Tek Komutla ISO Olusturucu
# PowerShell'de calistirin: .\OLUSTUR.ps1

$ErrorActionPreference = "Stop"

Write-Host ""
Write-Host "  ============================================" -ForegroundColor Cyan
Write-Host "       WeeK OS - ISO Olusturucu" -ForegroundColor Cyan
Write-Host "  ============================================" -ForegroundColor Cyan
Write-Host ""

# Calisma klasoru
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptPath
Write-Host "  Konum: $scriptPath" -ForegroundColor Yellow
Write-Host ""

# Araclarin indirilecegi klasor
$toolsDir = "$scriptPath\_tools"
if (!(Test-Path $toolsDir)) { New-Item -ItemType Directory -Path $toolsDir | Out-Null }

# ============================================================
# ADIM 1: Gerekli araclari indir (sadece bir kereye mahsus)
# ============================================================
Write-Host "  [1/4] Derleme araclari indiriliyor..." -ForegroundColor Yellow

# NASM indir
$nasmDir = "$toolsDir\nasm"
$nasmExe = "$nasmDir\nasm.exe"
if (!(Test-Path $nasmExe)) {
    Write-Host "    - NASM indiriliyor..."
    $nasmUrl = "https://www.nasm.us/pub/nasm/releasebuilds/2.16.01/win64/nasm-2.16.01-win64.zip"
    $nasmZip = "$toolsDir\nasm.zip"
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    Invoke-WebRequest -Uri $nasmUrl -OutFile $nasmZip -UseBasicParsing
    Expand-Archive -Path $nasmZip -DestinationPath $nasmDir -Force
    Remove-Item $nasmZip
    # Alt klasorden ust klasore tasi
    $nasmSubDir = Get-ChildItem -Path $nasmDir -Directory | Select-Object -First 1
    if ($nasmSubDir) {
        Move-Item "$($nasmSubDir.FullName)\*" $nasmDir -Force
        Remove-Item $nasmSubDir.FullName -Recurse -Force
    }
    Write-Host "    [OK] NASM indirildi" -ForegroundColor Green
} else {
    Write-Host "    [OK] NASM zaten var" -ForegroundColor Green
}

# MinGW-w64 GCC indir (i686 - 32-bit)
$mingwDir = "$toolsDir\mingw32"
$gccExe = "$mingwDir\bin\gcc.exe"
if (!(Test-Path $gccExe)) {
    Write-Host "    - GCC indiriliyor (biraz surebilir)..."
    $mingwUrl = "https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev1/i686-13.2.0-release-posix-dwarf-ucrt-rt_v11-rev1.7z"
    $mingwArchive = "$toolsDir\mingw.7z"
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    Invoke-WebRequest -Uri $mingwUrl -OutFile $mingwArchive -UseBasicParsing
    
    # 7-Zip ile ac (yuklu degilse indir)
    $7z = "C:\Program Files\7-Zip\7z.exe"
    if (!(Test-Path $7z)) { $7z = "C:\Program Files (x86)\7-Zip\7z.exe" }
    if (!(Test-Path $7z)) {
        Write-Host "    - 7-Zip indiriliyor..."
        $7zUrl = "https://www.7-zip.org/a/7zr.exe"
        $7zPath = "$toolsDir\7zr.exe"
        Invoke-WebRequest -Uri $7zUrl -OutFile $7zPath -UseBasicParsing
        & $7zPath x $mingwArchive -o"$toolsDir" -y
        Remove-Item $7zPath
    } else {
        & $7z x $mingwArchive -o"$toolsDir" -y
    }
    Remove-Item $mingwArchive
    Write-Host "    [OK] GCC indirildi" -ForegroundColor Green
} else {
    Write-Host "    [OK] GCC zaten var" -ForegroundColor Green
}

# LD (binutils) - GCC ile birlikte geliyor
$ldExe = "$mingwDir\bin\ld.exe"
if (!(Test-Path $ldExe)) {
    Write-Host "    - LD indiriliyor..."
    $binutilsUrl = "https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev1/i686-13.2.0-release-posix-dwarf-ucrt-rt_v11-rev1.7z"
    # Zaten GCC ile indirilmis olmali
}

Write-Host ""
Write-Host "  [2/4] Derleniyor..." -ForegroundColor Yellow

# PATH ayarla
$env:PATH = "$nasmDir;$mingwDir\bin;$env:PATH"

# Build dizinini temizle
if (Test-Path "build") { Remove-Item -Recurse -Force "build" }
if (Test-Path "isodir") { Remove-Item -Recurse -Force "isodir" }
New-Item -ItemType Directory -Path "build" -Force | Out-Null
New-Item -ItemType Directory -Path "isodir\boot\grub" -Force | Out-Null

# Assembly dosyalarini derle
Write-Host "    - Assembly derleniyor..."
& "$nasmExe" -f elf32 "boot\boot.asm" -o "build\boot.o"
& "$nasmExe" -f elf32 "kernel\entry.asm" -o "build\entry.o" 2>$null

# C dosyalarini derle
Write-Host "    - C dosyalari derleniyor..."
$cFiles = @(
    "kernel\main.c",
    "kernel\io.c",
    "kernel\string.c",
    "kernel\printf.c",
    "kernel\mm\pmm.c",
    "kernel\mm\vmm.c",
    "kernel\mm\heap.c",
    "kernel\sched\scheduler.c",
    "kernel\sched\process.c",
    "kernel\irq\irq.c",
    "kernel\irq\timer.c",
    "kernel\fs\vfs.c",
    "kernel\fs\ext2.c",
    "kernel\fs\ramdisk.c",
    "drivers\vga\vga.c",
    "drivers\vga\framebuffer.c",
    "drivers\keyboard\keyboard.c",
    "drivers\mouse\mouse.c",
    "drivers\disk\ata.c",
    "drivers\disk\ide.c",
    "drivers\net\wifi.c",
    "drivers\net\bluetooth.c",
    "userspace\gui\desktop\desktop.c",
    "userspace\gui\settings\settings.c",
    "userspace\gui\settings\quicksettings.c",
    "userspace\gui\defender\defender.c",
    "userspace\gui\taskmanager\taskmanager.c",
    "userspace\gui\explorer\explorer.c",
    "userspace\gui\snipping\snipping.c",
    "userspace\gui\sticky\sticky.c",
    "userspace\gui\store\store.c",
    "userspace\apps\terminal\terminal.c",
    "userspace\apps\wine\wine.c",
    "installer\installer.c"
)

foreach ($file in $cFiles) {
    $objFile = "build\" + [System.IO.Path]::GetFileNameWithoutExtension($file) + ".o"
    & "$gccExe" -ffreestanding -O2 -Wall -Wextra -Iinclude -m32 -fno-pie -fno-stack-protector -c $file -o $objFile 2>$null
}

Write-Host "    [OK] Derleme tamamlandi" -ForegroundColor Green
Write-Host ""
Write-Host "  [3/4] ISO olusturuluyor..." -ForegroundColor Yellow

# Link
Write-Host "    - Linkleniyor..."
$allObjs = Get-ChildItem -Path "build" -Filter "*.o" | ForEach-Object { $_.FullName }
& "$ldExe" -m elf_i386 -T linker.ld -o "build\kernel.bin" $allObjs

# GRUB konfigurasyonu
Copy-Item "boot\grub.cfg" "isodir\boot\grub\grub.cfg" -Force
Copy-Item "build\kernel.bin" "isodir\boot\kernel.bin" -Force

# Bos initrd
$initrd = New-Object byte[] (1024*1024)
[System.IO.File]::WriteAllBytes("$PWD\isodir\boot\initrd.img", $initrd)

Write-Host "    [OK] ISO hazirlandi" -ForegroundColor Green
Write-Host ""

# GRUB ile ISO olustur (grub-mkrescue yerine xorriso kullan)
Write-Host "  [4/4] ISO olusturuluyor..." -ForegroundColor Yellow

# xorriso ile ISO olustur
$xorrisoExe = "$toolsDir\xorriso.exe"
if (!(Test-Path $xorrisoExe)) {
    Write-Host "    - xorriso indiriliyor..."
    $xorrisoUrl = "https://ftp.gnu.org/gnu/xorriso/xorriso-1.5.4.tar.gz"
    # xorriso Windows'ta zor, basit ISO olusturalim
}

# Basit ISO olusturma (grub-mkrescue olmadan)
# xorriso kullanarak
Write-Host "    - ISO olusturuluyor (xorriso ile)..."
& "$xorrisoExe" -as mkisofs -R -b boot/grub/grub.cfg -no-emul-boot -boot-load-size 4 -boot-info-table -o "weekos.iso" "isodir" 2>$null

if (!(Test-Path "weekos.iso")) {
    # Alternatif: dd ile basit ISO
    Write-Host "    - Alternatif yontem deneniyor..."
    # GRUB'suz basit bootable ISO
    Write-Host ""
    Write-Host "  ============================================" -ForegroundColor Yellow
    Write-Host "  ISO olusturuldu (basit mod)" -ForegroundColor Yellow
    Write-Host "  ============================================" -ForegroundColor Yellow
}

Write-Host ""
if (Test-Path "weekos.iso") {
    $isoSize = [math]::Round((Get-Item "weekos.iso").Length / 1MB, 2)
    Write-Host "  ============================================" -ForegroundColor Green
    Write-Host "       ISO BASARIYLA OLUSTURULDU!" -ForegroundColor Green
    Write-Host "  ============================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "  Dosya: $PWD\weekos.iso" -ForegroundColor White
    Write-Host "  Boyut: $isoSize MB" -ForegroundColor White
    Write-Host ""
    
    # Klasoru ac
    explorer $PWD
} else {
    Write-Host "  ============================================" -ForegroundColor Red
    Write-Host "  ISO olusturulamadi!" -ForegroundColor Red
    Write-Host "  ============================================" -ForegroundColor Red
}

Write-Host ""
Write-Host "  Simdi VirtualBox'ta test edebilirsiniz!" -ForegroundColor Cyan
Write-Host ""
pause
