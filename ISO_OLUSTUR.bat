@echo off
title WeeK OS - Tek Tikla ISO Olusturucu
color 0B

echo.
echo  ╔══════════════════════════════════════════════╗
echo  ║       WeeK OS - ISO Olusturucu              ║
echo  ║       Tek tikla, kolayca ISO alin!          ║
echo  ╚══════════════════════════════════════════════╝
echo.

REM Docker kontrolü
docker --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo  [!] Docker bulunamadi!
    echo.
    echo  ════════════════════════════════════════════
    echo   Lutfen once Docker Desktop'i kurun:
    echo   https://docs.docker.com/desktop/install/windows-install/
    echo  ════════════════════════════════════════════
    echo.
    echo  Kurduktan sonra bilgisayarinizi yeniden baslatin
    echo  ve tekrar bu dosyaya tiklayin.
    echo.
    pause
    start https://docs.docker.com/desktop/install/windows-install/
    exit /b 1
)

echo  [✓] Docker bulundu!
echo  [~] ISO olusturuluyor... (2-5 dakika surebilir)
echo.

REM ISO olustur
docker build -t weekos-builder .
if %ERRORLEVEL% neq 0 (
    echo  [X] Hata: Docker imaji olusturulamadi!
    pause
    exit /b 1
)

docker run --rm -v "%CD%":/workspace weekos-builder ./build.sh
if %ERRORLEVEL% neq 0 (
    echo  [X] Hata: ISO olusturulamadi!
    pause
    exit /b 1
)

echo.
echo  ╔══════════════════════════════════════════════╗
echo  ║       ISO Basariyla Olusturuldu!            ║
echo  ╚══════════════════════════════════════════════╝
echo.
echo  ISO dosyasi: %CD%\weekos.iso
echo.

REM ISO'nun boyutunu goster
for %%A in (weekos.iso) do echo  Boyut: %%~zA byte
echo.

REM Klasoru ac
explorer .

echo  ════════════════════════════════════════════
echo   Simdi VirtualBox'ta test edebilirsiniz!
echo  ════════════════════════════════════════════
echo.
pause
