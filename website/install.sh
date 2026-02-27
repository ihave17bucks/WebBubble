#!/bin/bash
# WebBubble Installation Script 🫧
# Usage: curl -fsSL https://webbubble.dev/install.sh | sh

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# Configuration
REPO="yourusername/webbubble"
VERSION="latest"
INSTALL_DIR="/usr/local/bin"

echo -e "${PURPLE}"
echo "╔════════════════════════════════════════╗"
echo "║     WebBubble Installer 🫧            ║"
echo "║  Fast • Simple • Production Ready     ║"
echo "╚════════════════════════════════════════╝"
echo -e "${NC}"

# Detect OS and Architecture
OS=$(uname -s | tr '[:upper:]' '[:lower:]')
ARCH=$(uname -m)

case "$ARCH" in
    x86_64)
        ARCH="x64"
        ;;
    aarch64|arm64)
        ARCH="arm64"
        ;;
    *)
        echo -e "${RED}❌ Unsupported architecture: $ARCH${NC}"
        exit 1
        ;;
esac

echo -e "${BLUE}🔍 Detected: $OS-$ARCH${NC}"

# Check if running as root for system-wide install
if [ "$EUID" -ne 0 ] && [ -w "$INSTALL_DIR" ]; then
    SUDO=""
elif [ "$EUID" -ne 0 ]; then
    SUDO="sudo"
    echo -e "${YELLOW}⚠️  System-wide install requires sudo${NC}"
else
    SUDO=""
fi

# Function to install from source
install_from_source() {
    echo -e "${YELLOW}📦 No pre-built binary available, building from source...${NC}"
    
    # Check for required tools
    if ! command -v git >/dev/null 2>&1; then
        echo -e "${RED}❌ git is required but not installed${NC}"
        exit 1
    fi
    
    if ! command -v make >/dev/null 2>&1; then
        echo -e "${RED}❌ make is required but not installed${NC}"
        exit 1
    fi
    
    if ! command -v gcc >/dev/null 2>&1; then
        echo -e "${RED}❌ gcc is required but not installed${NC}"
        echo -e "${YELLOW}Install with:${NC}"
        if [ "$OS" = "darwin" ]; then
            echo "  xcode-select --install"
        else
            echo "  sudo apt-get install build-essential   # Ubuntu/Debian"
            echo "  sudo yum groupinstall 'Development Tools'   # CentOS/RHEL"
        fi
        exit 1
    fi
    
    # Clone and build
    TMP_DIR=$(mktemp -d)
    cd "$TMP_DIR"
    
    echo -e "${BLUE}📥 Cloning repository...${NC}"
    git clone https://github.com/$REPO.git
    cd webbubble
    
    echo -e "${BLUE}🔨 Building WebBubble...${NC}"
    make
    
    echo -e "${BLUE}📦 Installing...${NC}"
    $SUDO cp build/webbubble-server "$INSTALL_DIR/webbubble"
    $SUDO chmod +x "$INSTALL_DIR/webbubble"
    
    cd /
    rm -rf "$TMP_DIR"
    
    echo -e "${GREEN}✅ Built and installed from source!${NC}"
}

# Try to download pre-built binary (if available)
DOWNLOAD_URL="https://github.com/$REPO/releases/download/v$VERSION/webbubble-$OS-$ARCH"

echo -e "${BLUE}📥 Downloading WebBubble...${NC}"

if command -v curl >/dev/null 2>&1; then
    if curl -fsSL "$DOWNLOAD_URL" -o /tmp/webbubble 2>/dev/null; then
        echo -e "${GREEN}✅ Downloaded pre-built binary${NC}"
        $SUDO mv /tmp/webbubble "$INSTALL_DIR/webbubble"
        $SUDO chmod +x "$INSTALL_DIR/webbubble"
    else
        install_from_source
    fi
elif command -v wget >/dev/null 2>&1; then
    if wget -q "$DOWNLOAD_URL" -O /tmp/webbubble 2>/dev/null; then
        echo -e "${GREEN}✅ Downloaded pre-built binary${NC}"
        $SUDO mv /tmp/webbubble "$INSTALL_DIR/webbubble"
        $SUDO chmod +x "$INSTALL_DIR/webbubble"
    else
        install_from_source
    fi
else
    echo -e "${RED}❌ Neither curl nor wget found${NC}"
    exit 1
fi

# Verify installation
if command -v webbubble >/dev/null 2>&1; then
    echo ""
    echo -e "${GREEN}╔════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║   WebBubble installed successfully! 🫧 ║${NC}"
    echo -e "${GREEN}╚════════════════════════════════════════╝${NC}"
    echo ""
    echo -e "${BLUE}📍 Installed to: $INSTALL_DIR/webbubble${NC}"
    echo -e "${BLUE}📝 Version: $(webbubble --version 2>/dev/null || echo 'v1.1.0')${NC}"
    echo ""
    echo -e "${PURPLE}🚀 Quick Start:${NC}"
    echo ""
    echo -e "  ${YELLOW}# Create a hello.bub file${NC}"
    echo -e "  echo 'route \"/\" { response \"Hello! 🫧\" }' > hello.bub"
    echo ""
    echo -e "  ${YELLOW}# Run it${NC}"
    echo -e "  webbubble hello.bub"
    echo ""
    echo -e "  ${YELLOW}# Visit http://localhost:8080${NC}"
    echo ""
    echo -e "${BLUE}📚 Documentation: https://webbubble.dev/docs${NC}"
    echo -e "${BLUE}💬 Discord: https://discord.gg/webbubble${NC}"
    echo -e "${BLUE}⭐ GitHub: https://github.com/$REPO${NC}"
    echo ""
else
    echo -e "${RED}❌ Installation failed${NC}"
    echo -e "${YELLOW}Please check that $INSTALL_DIR is in your PATH${NC}"
    exit 1
fi
