@echo off
cls
echo.
echo  ISO olusturuluyor... Lutfen bekleyin (2-5 dk)
echo.
C:\msys64\usr\bin\bash.exe -lc "export PATH='/mingw32/bin:$PATH' && cd '/c/Users/Kerem/Desktop/weekos_build' && bash build.sh"
echo.
if exist "C:\Users\Kerem\Desktop\weekos_build\weekos.iso" (
    copy "C:\Users\Kerem\Desktop\weekos_build\weekos.iso" "C:\Users\Kerem\Desktop\weekos.iso" >nul
    echo  ISO HAZIR! Masaustunde: weekos.iso
    explorer "C:\Users\Kerem\Desktop"
) else (
    echo  HATA: ISO olusturulamadi
)
echo.
pause
