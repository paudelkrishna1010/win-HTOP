# ==============================
# win-HTOP build & run script
# ==============================

# Stop on error
$ErrorActionPreference = "Stop"

# Project root (go one level up from scripts/)
$ROOT = Resolve-Path "$PSScriptRoot\.."

# Paths
$SRC = "$ROOT\src"
$INCLUDE = "$ROOT\include"
$BUILD = "$ROOT\build"
$OUT = "$BUILD\win-htop.exe"

# Create build directory if not exists
if (!(Test-Path $BUILD)) {
    New-Item -ItemType Directory -Path $BUILD | Out-Null
}

Write-Host "Building win-HTOP..." -ForegroundColor Cyan

# Compile all cpp files
g++ `
    -std=c++17 `
    -I"$INCLUDE" `
    "$SRC\*.cpp" `
    -o "$OUT" `
    -lpsapi

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed." -ForegroundColor Red
    exit 1
}

Write-Host "Build successful." -ForegroundColor Green
Write-Host "Running..." -ForegroundColor Cyan
Write-Host ""

# Run executable
& "$OUT"
