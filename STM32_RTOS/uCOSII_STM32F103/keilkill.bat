@echo off
rem Remove Keil build artifacts, keep source files.
setlocal
cd /d "%~dp0"

if exist "MDK-ARM\Objects" (
    del /q "MDK-ARM\Objects\*.*" 2>nul
)
if exist "MDK-ARM\Listings" (
    del /q "MDK-ARM\Listings\*.*" 2>nul
)
if exist "MDK-ARM\*.uvguix.*" (
    del /q "MDK-ARM\*.uvguix.*" 2>nul
)
if exist "MDK-ARM\*.scvd" (
    del /q "MDK-ARM\*.scvd" 2>nul
)

echo Build artifacts removed.
pause
