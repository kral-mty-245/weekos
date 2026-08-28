@echo off
REM ============================================
REM  WeeK OS - Windows VirtualBox Setup
REM ============================================

echo.
echo ===========================================
echo   WeeK OS VirtualBox Setup (Windows)
echo ===========================================
echo.

REM VirtualBox kontrolü
where VBoxManage >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo HATA: VirtualBox bulunamadi!
    echo.
    echo VirtualBox'i indirin:
    echo   https://www.virtualbox.org/wiki/Downloads
    echo.
    pause
    exit /b 1
)

REM ISO kontrolü
if not exist weekos.iso (
    echo HATA: weekos.iso bulunamadi!
    echo.
    echo Once ISO'yu olusturun (Linux'ta veya Docker ile)
    echo.
    pause
    exit /b 1
)

echo [1/5] Sanal makine olusturuluyor...

REM Eski makineyi sil
VBoxManage unregistervm "WeeK OS" --delete 2>nul

REM Yeni sanal makine
VBoxManage createvm --name "WeeK OS" --ostype "Linux26_64" --register --basefolder "%CD%"

echo [2/5] Yapilandirma ayarlaniyor...
VBoxManage modifyvm "WeeK OS" --memory 2048 --cpus 2 --vram 128 --graphicscontroller vmsvga
VBoxManage modifyvm "WeeK OS" --nic1 nat --nictype1 82540EM
VBoxManage modifyvm "WeeK OS" --boot1 dvd --boot2 disk

echo [3/5] Sanal disk olusturuluyor...
VBoxManage storagectl "WeeK OS" --name "SATA" --add sata
VBoxManage createmedium disk --filename "%CD%\WeeK OS\WeeK OS.vdi" --size 20480 --format VDI
VBoxManage storageattach "WeeK OS" --storagectl "SATA" --port 0 --device 0 --type hdd --medium "%CD%\WeeK OS\WeeK OS.vdi"

echo [4/5] ISO baglaniyor...
VBoxManage storagectl "WeeK OS" --name "IDE" --add ide
VBoxManage storageattach "WeeK OS" --storagectl "IDE" --port 0 --device 0 --type dvddrive --medium "%CD%\weekos.iso"

echo [5/5] Sanal makine baslatiliyor...
VBoxManage startvm "WeeK OS" --type gui

echo.
echo ===========================================
echo   WeeK OS VirtualBox Kurulum Tamamlandi!
echo ===========================================
echo.
echo   Sanal Makine: WeeK OS
echo   RAM: 2048MB
echo   Disk: 20GB
echo   ISO: weekos.iso
echo.
echo   WeeK OS simdi basliyor!
echo.
pause
