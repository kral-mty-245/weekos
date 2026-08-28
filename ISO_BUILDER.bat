@echo off
title WeeK OS - ISO Builder
color 0A
mode con: cols=60 lines=30
cls

echo.
echo  ============================================
echo       WeeK OS - Tek Tikla ISO Builder
echo  ============================================
echo.
echo  ISO olusturuluyor... Lutfen bekleyin.
echo.

REM Hedef klasor
set "TARGET=C:\Users\%USERNAME%\Desktop\weekos_build"

REM Klasor zaten varsa sil ve yeniden kopyala
if exist "%TARGET%" rmdir /s /q "%TARGET%" 2>nul

echo  [1/4] Proje kopyalaniyor...
mkdir "%TARGET%" 2>nul

REM Orjinal klasorden kopyala
xcopy "C:\Users\Kerem\OneDrive\Yapay Zekalarla Uygulamalar!\Diğer\Freebuff\WeeKOS\weekos\*" "%TARGET%\" /E /I /Q /Y >nul 2>&1

if not exist "%TARGET%\build.sh" (
    echo  [HATA] Kopyalama basarisiz!
    echo  Lutfen weekos klasorunu masaustune kopyalayin
    echo  ve "weekos_build" olarak yeniden adlandirin.
    pause
    exit /b 1
)

echo  [OK] Kopyalandi
echo.
echo  [2/4] Derleniyor... (2-5 dk surebilir)
echo.

REM MSYS2 bash ile build et
C:\msys64\usr\bin\bash.exe -lc "export PATH='/mingw32/bin:$PATH' && cd '/c/Users/%USERNAME%/Desktop/weekos_build' && bash build.sh"

echo.
echo  [3/4] Kontrol ediliyor...

if exist "%TARGET%\weekos.iso" (
    copy "%TARGET%\weekos.iso" "C:\Users\%USERNAME%\Desktop\weekos.iso" >nul
    
    echo.
    echo  ============================================
    echo       ISO HAZIR!
    echo  ============================================
    echo.
    echo  Dosya: C:\Users\%USERNAME%\Desktop\weekos.iso
    echo.
    explorer "C:\Users\%USERNAME%\Desktop"
) else (
    echo.
    echo  ============================================
    echo   HATA: ISO olusturulamadi
    echo  ============================================
)

echo.
pause
