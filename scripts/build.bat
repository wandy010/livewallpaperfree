@echo off
REM ============================================================================
REM  BUILD + INSTALL — WallpaperEngineClone
REM ============================================================================
REM  Requisitos:
REM    1. Visual Studio 2019+ (con "Desktop development with C++")
REM    2. CMake 3.15+        (https://cmake.org/download/)
REM ============================================================================

setlocal EnableDelayedExpansion

REM ── 1. Definir Arquitectura ──────────────────────────────────────────────────
set ARCH=x64
echo [INFO] Compilando como !ARCH!

REM ── 2. Configurar con CMake ──────────────────────────────────────────────────
echo [STEP] Configurando con CMake (-A !ARCH!)...
cmake -B build -G "Visual Studio 17 2022" -A %ARCH%
if %ERRORLEVEL% neq 0 goto :error

REM ── 3. Compilar ────────────────────────────────────────────────────────────
echo [STEP] Compilando...
cmake --build build --config Release
if %ERRORLEVEL% neq 0 goto :error

REM ── 4. Copiar assets ────────────────────────────────────────────────────────
echo [STEP] Preparando directorio de salida...
set OUTDIR=build\Release

REM assets
xcopy assets "%OUTDIR%\assets\" /E /I /Y >nul

REM ── 5. Generar instalador NSIS ─────────────────────────────────────────────
echo [STEP] Generando instalador...
cd build
cpack -C Release 2>nul
if exist WallpaperEngineClone-*.exe (
    move WallpaperEngineClone-*.exe ..\ >nul
    echo [OK] Instalador creado: WallpaperEngineClone-*.exe
)
cd ..

echo ============================================================================
echo  LISTO!
echo ============================================================================
echo  Ejecutable: %OUTDIR%\WallpaperEngineClone.exe
echo  Assets:     %OUTDIR%\assets\
echo  Instalador: WallpaperEngineClone-*.exe
echo ============================================================================
echo.
echo  Para EJECUTAR:
echo    %OUTDIR%\WallpaperEngineClone.exe
echo ============================================================================
goto :eof

:error
echo [ERROR] Fallo en el paso anterior. Revisa los mensajes.
exit /b 1
