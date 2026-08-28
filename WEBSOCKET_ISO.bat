@echo off
title WeeK OS - ISO Olusturucu
color 0A

echo.
echo  ╔══════════════════════════════════════════════╗
echo  ║       WeeK OS - ISO Olusturucu              ║
echo  ║       Kolayca ISO olusturun!                ║
echo  ╚══════════════════════════════════════════════╝
echo.

REM WSL kontrolü
wsl --list >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo  [~] WSL kuruluyor... (Bir kereye mahsus)
    echo.
    wsl --install
    echo.
    echo  ════════════════════════════════════════════
    echo   Bilgisayarinizi yeniden baslatin!
    echo   Sonra tekrar bu dosyaya tiklayin.
    echo  ════════════════════════════════════════════
    pause
    exit /b 0
)

echo  [✓] WSL hazir!
echo  [~] Build araclari kuruluyor...
echo.

REM Build araçlarını kur
wsl -e bash -c "sudo apt-get update -qq && sudo apt-get install -y -qq build-essential nasm grub-pc-bin grub-common xorriso mtools > /dev/null 2>&1"
echo  [✓] Build araclari hazir!
echo.

REM Projeyi WSL'ye kopyala
echo  [~] Proje kopyalaniyor...
wsl -e bash -c "rm -rf ~/weekos 2>/dev/null; cp -r /mnt/c/Users/Kerem/Desktop/weekos ~/ 2>/dev/null || cp -r /mnt/c/Users/*/Desktop/weekos ~/ 2>/dev/null || echo 'Proje bulunamadi'"
echo.

REM ISO olustur
echo  [~] ISO olusturuluyor... (2-5 dakika surebilir)
echo.
wsl -e bash -c "cd ~/weekos && chmod +x build.sh && ./build.sh"
echo.

REM ISO'yu Windows'a kopyala
echo  [~] ISO kopyalaniyor...
wsl -e bash -c "cp ~/weekos/weekos.iso /mnt/c/Users/Kerem/Desktop/ 2>/dev/null || cp ~/weekos/weekos.iso /mnt/c/Users/*/Desktop/ 2>/dev/null"
echo.

echo  ╔══════════════════════════════════════════════╗
echo  ║       ISO Basariyla Olusturuldu!            ║
echo  ╚══════════════════════════════════════════════╝
echo.
echo  ISO dosyasi masaustunuzde: weekos.iso
echo.
echo  ════════════════════════════════════════════
echo   Simdi VirtualBox'ta test edebilirsiniz!
echo  ════════════════════════════════════════════
echo.
pause
