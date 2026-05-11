# ============================================================================
# BUILD + INSTALL — WallpaperEngineClone (PowerShell)
# ============================================================================
# Requisitos:
#   1. Visual Studio 2019+  (Desktop development with C++)
#   2. CMake 3.15+          (choco install cmake)
# ============================================================================

$ErrorActionPreference = "Stop"

# ── 1. Configurar ────────────────────────────────────────────────────────────
Write-Host "[STEP] Configurando..." -ForegroundColor Cyan
& cmake -B build -G "Visual Studio 17 2022" -A x64
if (-not $?) { throw "CMake configure fallo" }

# ── 2. Compilar ──────────────────────────────────────────────────────────────
Write-Host "[STEP] Compilando..." -ForegroundColor Cyan
& cmake --build build --config Release
if (-not $?) { throw "Build fallo" }

# ── 3. Copiar assets ─────────────────────────────────────────────────────────
Write-Host "[STEP] Copiando assets..." -ForegroundColor Cyan
$OutDir = "build\Release"
Copy-Item "assets" -Destination "$OutDir\assets" -Recurse -Force

# ── 4. Generar instalador NSIS ───────────────────────────────────────────────
Write-Host "[STEP] Generando instalador..." -ForegroundColor Cyan
Push-Location build
& cpack -C Release 2>$null
Get-ChildItem "WallpaperEngineClone-*.exe" | Move-Item -Destination .. -Force
Pop-Location

Write-Host "========================================" -ForegroundColor Green
Write-Host "  LISTO!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host "Ejecutable: $OutDir\WallpaperEngineClone.exe"
Write-Host "Assets:     $OutDir\assets\"
Write-Host "Instalador: WallpaperEngineClone-*.exe"
Write-Host ""
Write-Host "Para ejecutar:"
Write-Host "  $OutDir\WallpaperEngineClone.exe"
