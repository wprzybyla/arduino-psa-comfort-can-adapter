# Build script that uses PlatformIO's built-in Python
# This avoids Windows Store Python issues
#
# Usage:
#   .\build.ps1              # Build project
#   .\build.ps1 --target upload    # Build and upload
#   .\build.ps1 --target clean     # Clean build
#   .\build.ps1 --target monitor   # Monitor serial output

# Add PlatformIO Python to PATH (before Windows Store Python)
$platformioPython = "$env:USERPROFILE\.platformio\penv\Scripts"
if (Test-Path $platformioPython) {
    $env:PATH = "$platformioPython;" + ($env:PATH -replace [regex]::Escape("$env:LOCALAPPDATA\Microsoft\WindowsApps"), "")
} else {
    Write-Host "Warning: PlatformIO Python not found at $platformioPython" -ForegroundColor Yellow
}

Write-Host "=== PlatformIO Build Script ===" -ForegroundColor Cyan
Write-Host "Using Python: $(python --version 2>&1)" -ForegroundColor Green
Write-Host "Python path: $(where.exe python 2>&1)" -ForegroundColor Green
Write-Host ""

# Run PlatformIO with all passed arguments
if ($args.Count -eq 0) {
    Write-Host "Building project..." -ForegroundColor Cyan
    pio run
} else {
    Write-Host "Running: pio $($args -join ' ')" -ForegroundColor Cyan
    pio $args
}
