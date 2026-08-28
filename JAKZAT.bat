@echo off
title WeeK OS - ISO Al
color 0A
mode con: cols=60 lines=35

cls
echo.
echo  ============================================
echo           WeeK OS - ISO Olusturucu
echo  ============================================
echo.
echo.
echo   Bu dosya ISO olusturur.
echo.
echo  --------------------------------------------
echo.
echo   ONCELIKLE:
echo.
echo   1. Docker Desktop'i kurun
echo      https://docs.docker.com/desktop/install/windows-install/
echo.
echo   2. Docker Desktop'i acin
echo      (Sistem tepsisinde YESIL ikon olmali!)
echo.
echo   3. Bu dosyaya cift tiklayin
echo.
echo  --------------------------------------------
echo.
set /p devam="  Devam icin Enter'a basin... "

echo.
echo  [1/3] Docker kontrol ediliyor...
docker --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo.
    echo  ============================================
    echo   HATA: Docker bulunamadi!
    echo   Docker Desktop'i kurun.
    echo  ============================================
    echo.
    start https://docs.docker.com/desktop/install/windows-install/
    pause
    exit /b
)

REM Docker daemon calisiyor mu kontrol et
docker ps >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo.
    echo  ============================================
    echo   HATA: Docker calismiyor!
    echo.
    echo   1. Docker Desktop'i acin
    echo   2. Sistem tepsisinde YESIL ikon olana
    echo      kadar bekleyin
    echo   3. Sonra bu dosyaya tekrar tiklayin
    echo  ============================================
    echo.
    pause
    exit /b
)

echo  [OK] Docker hazir
echo.
echo  [2/3] ISO olusturuluyor... (2-5 dk surebilir)
echo.
docker build -t weekos-builder . >nul 2>&1
docker run --rm -v "%CD%":/workspace weekos-builder ./build.sh

echo.
if exist weekos.iso (
    echo  ============================================
    echo   ISO HAZIR!
    echo  ============================================
    echo.
    echo  [3/3] ISO: %CD%\weekos.iso
    echo.
    explorer .
) else (
    echo  ============================================
    echo   HATA: ISO olusturulamadi!
    echo  ============================================
)

echo.
pause
