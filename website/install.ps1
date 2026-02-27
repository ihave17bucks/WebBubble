# WebBubble Installation Script for Windows 🫧
# Usage: iwr https://webbubble.dev/install.ps1 -useb | iex

$ErrorActionPreference = "Stop"

# Configuration
$repo = "yourusername/webbubble"
$version = "latest"
$installDir = "$env:LOCALAPPDATA\WebBubble"
$binPath = "$installDir\bin"

Write-Host @"

╔════════════════════════════════════════╗
║     WebBubble Installer 🫧            ║
║  Fast • Simple • Production Ready     ║
╚════════════════════════════════════════╝

"@ -ForegroundColor Magenta

Write-Host "🔍 Detected: Windows $(if ([Environment]::Is64BitOperatingSystem) { 'x64' } else { 'x86' })" -ForegroundColor Blue

# Detect architecture
$arch = if ([Environment]::Is64BitOperatingSystem) { "x64" } else { "x86" }

# Create installation directory
if (-not (Test-Path $binPath)) {
    New-Item -ItemType Directory -Path $binPath -Force | Out-Null
    Write-Host "📁 Created installation directory: $binPath" -ForegroundColor Blue
}

# Download URL
$downloadUrl = "https://github.com/$repo/releases/download/v$version/webbubble-windows-$arch.exe"

Write-Host "📥 Downloading WebBubble..." -ForegroundColor Blue

try {
    # Try to download pre-built binary
    $webClient = New-Object System.Net.WebClient
    $webClient.DownloadFile($downloadUrl, "$binPath\webbubble.exe")
    Write-Host "✅ Downloaded pre-built binary" -ForegroundColor Green
} catch {
    Write-Host "⚠️  Pre-built binary not available" -ForegroundColor Yellow
    Write-Host "📦 Building from source..." -ForegroundColor Yellow
    
    # Check for git
    if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
        Write-Host "❌ git is required but not installed" -ForegroundColor Red
        Write-Host "Install from: https://git-scm.com/download/win" -ForegroundColor Yellow
        exit 1
    }
    
    # Check for build tools
    if (-not (Get-Command gcc -ErrorAction SilentlyContinue)) {
        Write-Host "❌ C++ compiler is required but not installed" -ForegroundColor Red
        Write-Host "Install MinGW-w64 from: https://www.msys2.org/" -ForegroundColor Yellow
        exit 1
    }
    
    # Clone and build
    $tempDir = Join-Path $env:TEMP "webbubble-build"
    if (Test-Path $tempDir) {
        Remove-Item -Recurse -Force $tempDir
    }
    
    Write-Host "📥 Cloning repository..." -ForegroundColor Blue
    git clone "https://github.com/$repo.git" $tempDir
    Set-Location $tempDir
    
    Write-Host "🔨 Building WebBubble..." -ForegroundColor Blue
    & .\build.bat
    
    if ($LASTEXITCODE -eq 0) {
        Copy-Item "build\webbubble-server.exe" "$binPath\webbubble.exe"
        Write-Host "✅ Built and installed from source!" -ForegroundColor Green
    } else {
        Write-Host "❌ Build failed" -ForegroundColor Red
        exit 1
    }
    
    Set-Location $env:USERPROFILE
    Remove-Item -Recurse -Force $tempDir
}

# Add to PATH if not already there
$currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($currentPath -notlike "*$binPath*") {
    Write-Host "🔧 Adding to PATH..." -ForegroundColor Blue
    [Environment]::SetEnvironmentVariable(
        "Path",
        "$currentPath;$binPath",
        "User"
    )
    $env:Path = "$env:Path;$binPath"
    Write-Host "✅ Added to PATH (restart terminal to use)" -ForegroundColor Green
} else {
    Write-Host "✅ Already in PATH" -ForegroundColor Green
}

# Verify installation
$webbubblePath = Join-Path $binPath "webbubble.exe"
if (Test-Path $webbubblePath) {
    Write-Host @"

╔════════════════════════════════════════╗
║   WebBubble installed successfully! 🫧 ║
╚════════════════════════════════════════╝

"@ -ForegroundColor Green

    Write-Host "📍 Installed to: $binPath\webbubble.exe" -ForegroundColor Blue
    Write-Host "📝 Version: v1.1.0" -ForegroundColor Blue
    Write-Host ""
    Write-Host "🚀 Quick Start:" -ForegroundColor Magenta
    Write-Host ""
    Write-Host "  # Create a hello.bub file" -ForegroundColor Yellow
    Write-Host "  echo 'route `"/`" { response `"Hello! 🫧`" }' > hello.bub"
    Write-Host ""
    Write-Host "  # Run it (restart terminal first if PATH was just updated)" -ForegroundColor Yellow
    Write-Host "  webbubble hello.bub"
    Write-Host ""
    Write-Host "  # Visit http://localhost:8080" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "📚 Documentation: https://webbubble.dev/docs" -ForegroundColor Blue
    Write-Host "💬 Discord: https://discord.gg/webbubble" -ForegroundColor Blue
    Write-Host "⭐ GitHub: https://github.com/$repo" -ForegroundColor Blue
    Write-Host ""
    
    if ($currentPath -notlike "*$binPath*") {
        Write-Host "⚠️  Please restart your terminal to use 'webbubble' command" -ForegroundColor Yellow
    }
} else {
    Write-Host "❌ Installation failed" -ForegroundColor Red
    exit 1
}
