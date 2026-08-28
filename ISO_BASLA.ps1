# WeeK OS - Tek Tikla ISO Olusturucu
# Bu dosyaya cift tiklayin veya PowerShell'de calistirin

Write-Host ""
Write-Host "  ╔══════════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "  ║       WeeK OS - ISO Olusturucu              ║" -ForegroundColor Cyan
Write-Host "  ║       Kolayca ISO olusturun!                ║" -ForegroundColor Cyan
Write-Host "  ╚══════════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""

# Calisma klasorunu ayarla
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptPath

Write-Host "  [~] Konum: $scriptPath" -ForegroundColor Yellow
Write-Host ""

# Docker kontrolü
Write-Host "  [1/4] Docker kontrol ediliyor..." -ForegroundColor Yellow
try {
    $dockerVersion = docker --version 2>$null
    if ($dockerVersion) {
        Write-Host "  [✓] Docker bulundu: $dockerVersion" -ForegroundColor Green
    } else {
        throw "Docker not found"
    }
} catch {
    Write-Host ""
    Write-Host "  ╔══════════════════════════════════════════════╗" -ForegroundColor Red
    Write-Host "  ║  Docker bulunamadi!                         ║" -ForegroundColor Red
    Write-Host "  ╚══════════════════════════════════════════════╝" -ForegroundColor Red
    Write-Host ""
    Write-Host "  Lutfen Docker Desktop'i kurun:" -ForegroundColor White
    Write-Host "  https://docs.docker.com/desktop/install/windows-install/" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "  Kurduktan sonra:" -ForegroundColor White
    Write-Host "  1. Bilgisayari yeniden baslatin" -ForegroundColor Yellow
    Write-Host "  2. Docker Desktop'i acin" -ForegroundColor Yellow
    Write-Host "  3. Bu dosyaya tekrar tiklayin" -ForegroundColor Yellow
    Write-Host ""
    
    $answer = Read-Host "  Docker sayfasini acayim mi? (E/H)"
    if ($answer -eq "E" -or $answer -eq "e") {
        Start-Process "https://docs.docker.com/desktop/install/windows-install/"
    }
    pause
    exit
}

Write-Host ""

# Docker imajını olustur
Write-Host "  [2/4] Docker imaji olusturuluyor..." -ForegroundColor Yellow
docker build -t weekos-builder . 2>&1 | Out-Null
if ($LASTEXITCODE -ne 0) {
    Write-Host "  [X] Hata: Docker imaji olusturulamadi!" -ForegroundColor Red
    pause
    exit 1
}
Write-Host "  [✓] Docker imaji hazir" -ForegroundColor Green

Write-Host ""

# ISO olustur
Write-Host "  [3/4] ISO olusturuluyor... (2-5 dakika surebilir)" -ForegroundColor Yellow
Write-Host ""
docker run --rm -v "${PWD}:/workspace" weekos-builder ./build.sh
if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "  [X] Hata: ISO olusturulamadi!" -ForegroundColor Red
    pause
    exit 1
}

Write-Host ""

# Sonuc
if (Test-Path "weekos.iso") {
    $isoSize = (Get-Item "weekos.iso").Length / 1MB
    Write-Host "  ╔══════════════════════════════════════════════╗" -ForegroundColor Green
    Write-Host "  ║       ISO Basariyla Olusturuldu!            ║" -ForegroundColor Green
    Write-Host "  ╚══════════════════════════════════════════════╝" -ForegroundColor Green
    Write-Host ""
    Write-Host "  ISO dosyasi: $PWD\weekos.iso" -ForegroundColor White
    Write-Host "  Boyut: $([math]::Round($isoSize, 2)) MB" -ForegroundColor White
    Write-Host ""
    Write-Host "  ════════════════════════════════════════════" -ForegroundColor Cyan
    Write-Host "  Simdi VirtualBox'ta test edebilirsiniz!" -ForegroundColor Green
    Write-Host "  ════════════════════════════════════════════" -ForegroundColor Cyan
    Write-Host ""
    
    # Klasoru ac
    explorer $PWD
} else {
    Write-Host "  [X] ISO dosyasi olusturulamadi!" -ForegroundColor Red
}

Write-Host ""
pause
