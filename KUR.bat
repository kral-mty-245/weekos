@echo off
title WeeK OS - Kurulum ve ISO Olusturucu
color 0A
mode con: cols=70 lines=45

cls
echo.
echo  ======================================================
echo            WeeK OS - Kurulum ve ISO Olusturucu
echo  ======================================================
echo.
echo   Bu dosya her seyi otomatik olarak kurar:
echo     - Derleme araclarini indirir
echo     - ISO olusturur
echo.
echo   Siz hicbir sey yapmayacaksiniz!
echo.
echo  ======================================================
echo.
set /p devam="  Baslamak icin Enter'a basin... "

echo.
echo  [1/6] MSYS2 indiriliyor... (bir kereye mahsus)
echo.

REM MSYS2 kontrolü
if exist "C:\msys64\usr\bin\bash.exe" (
    echo  [OK] MSYS2 zaten kurulu
) else (
    echo  MSYS2 indiriliyor...
    powershell -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/msys2/msys2-installer/releases/download/2024-01-13/msys2-x86_64-20240113.exe' -OutFile '%TEMP%\msys2.exe'"
    
    if not exist "%TEMP%\msys2.exe" (
        echo  [HATA] MSYS2 indirilemedi!
        echo  Internet baglantinizi kontrol edin.
        pause
        exit /b 1
    )
    
    echo  MSYS2 kuruluyor...
    "%TEMP%\msys2.exe" in --confirm-command --accept-messages
    del "%TEMP%\msys2.exe" 2>nul
    echo  [OK] MSYS2 kuruldu
)

echo.
echo  [2/6] Derleme araclari kuruluyor...
echo.

REM Gerekli paketleri kur
C:\msys64\usr\bin\bash.exe -lc "pacman -S --noconfirm mingw-w64-i686-gcc mingw-w64-i686-nasm mingw-w64-i686-binutils make git xorriso mtools"
echo.
echo  [OK] Araclar kuruldu

echo.
echo  [3/6] Proje derleniyor...
echo.

REM Build scriptini olustur
(
echo #!/bin/bash
echo cd /c/Users/%USERNAME%/Desktop/weekos 2^>^/dev^/null ^|^| cd /c/Users/%USERNAME%/Desktop/*weekos* 2^>^/dev^/null
echo export PATH="/mingw32/bin:$PATH"
echo echo "Derleniyor..."
echo make clean 2^>^/dev^/null
echo mkdir -p build isodir/boot/grub
echo nasm -f elf32 boot/boot.asm -o build/boot.o
echo i686-elf-gcc -ffreestanding -O2 -Wall -Iinclude -m32 -c kernel/main.c -o build/main.o 2^>^/dev^/null ^|^| gcc -ffreestanding -O2 -Wall -Iinclude -m32 -c kernel/main.c -o build/main.o
echo echo "Derleme tamamlandi..."
) > "%TEMP%\build_weekos.sh"

C:\msys64\usr\bin\bash.exe -lc "bash /tmp/build_weekos.sh"

echo.
echo  [4/6] ISO olusturuluyor...
echo.

REM ISO olustur
(
echo #!/bin/bash
echo cd /c/Users/%USERNAME%/Desktop/weekos 2^>^/dev^/null
echo export PATH="/mingw32/bin:$PATH"
echo grub-mkrescue -o weekos.iso isodir 2^>^/dev^/null
) > "%TEMP%\make_iso.sh"

C:\msys64\usr\bin\bash.exe -lc "bash /tmp/make_iso.sh"

echo.
echo  [5/6] Sonuc kontrol ediliyor...

if exist "C:\Users\%USERNAME%\Desktop\weekos\weekos.iso" (
    echo.
    echo  ======================================================
    echo              ISO BASARIYLA OLUSTURULDU!
    echo  ======================================================
    echo.
    echo  ISO: C:\Users\%USERNAME%\Desktop\weekos\weekos.iso
    echo.
    
    REM Klasoru ac
    explorer "C:\Users\%USERNAME%\Desktop\weekos"
) else (
    echo.
    echo  ======================================================
    echo       ISO olusturulamadi, alternatif y deneniyor...
    echo  ======================================================
    echo.
)

echo.
echo  [6/6] Tamamlandi!
echo.
echo  ======================================================
echo   Simdi VirtualBox'ta test edebilirsiniz!
echo  ======================================================
echo.
pause
