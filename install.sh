#!/bin/bash
# Installation script for Hardware Stress Test Suite
# MIT License (c) 2026 kj-devvixon

set -e

echo "=========================================="
echo " Hardware Stress Test Suite - Installer"
echo " MIT License © 2026 kj-devvixon"
echo "=========================================="
echo ""

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    echo "This script requires root privileges."
    echo "Please run with sudo: sudo ./install.sh"
    exit 1
fi

# Get the actual user (not root when using sudo)
ACTUAL_USER="${SUDO_USER:-$USER}"
ACTUAL_HOME=$(eval echo ~$ACTUAL_USER)

echo "Installing for user: $ACTUAL_USER"
echo ""

# Check for required tools
echo "Checking dependencies..."

if ! command -v g++ &> /dev/null; then
    echo "Error: g++ not found!"
    echo "Install with: sudo apt-get install build-essential"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "Error: make not found!"
    echo "Install with: sudo apt-get install build-essential"
    exit 1
fi

echo "✓ All dependencies found"
echo ""

# Build the application
echo "Building application..."
if [ -f "./stress_test" ]; then
    echo "Executable already exists, using existing build"
else
    if [ -f "./Makefile" ]; then
        sudo -u $ACTUAL_USER make
    else
        echo "Error: Makefile not found!"
        exit 1
    fi
fi

if [ ! -f "./stress_test" ]; then
    echo "Error: Build failed!"
    exit 1
fi

echo "✓ Build successful"
echo ""

# Install binary
echo "Installing binary..."
INSTALL_DIR="/usr/local/bin"
cp ./stress_test $INSTALL_DIR/stress_test
chmod 755 $INSTALL_DIR/stress_test

echo "✓ Binary installed to $INSTALL_DIR/stress_test"
echo ""

# Create systemd service (optional)
read -p "Install as systemd service (run on boot)? [y/N]: " install_service

if [ "$install_service" = "y" ] || [ "$install_service" = "Y" ]; then
    echo ""
    echo "Creating systemd service..."
    
    cat > /etc/systemd/system/stress-test.service << EOF
[Unit]
Description=Hardware Stress Test Suite Web Server
After=network.target

[Service]
Type=simple
User=$ACTUAL_USER
WorkingDirectory=$ACTUAL_HOME
ExecStart=/usr/local/bin/stress_test 8080
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
EOF

    systemctl daemon-reload
    systemctl enable stress-test.service
    
    echo "✓ Systemd service created and enabled"
    echo ""
    
    read -p "Start service now? [y/N]: " start_now
    if [ "$start_now" = "y" ] || [ "$start_now" = "Y" ]; then
        systemctl start stress-test.service
        echo "✓ Service started"
        echo ""
        echo "Access the web interface at: http://localhost:8080"
    else
        echo ""
        echo "Start service with: sudo systemctl start stress-test.service"
    fi
    
    echo ""
    echo "Service commands:"
    echo "  sudo systemctl start stress-test    # Start service"
    echo "  sudo systemctl stop stress-test     # Stop service"
    echo "  sudo systemctl status stress-test   # Check status"
    echo "  sudo systemctl disable stress-test  # Disable auto-start"
fi

# Create desktop entry (optional)
read -p "Create desktop shortcut? [y/N]: " create_desktop

if [ "$create_desktop" = "y" ] || [ "$create_desktop" = "Y" ]; then
    echo ""
    echo "Creating desktop entry..."
    
    DESKTOP_DIR="$ACTUAL_HOME/.local/share/applications"
    mkdir -p $DESKTOP_DIR
    
    cat > $DESKTOP_DIR/stress-test.desktop << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Hardware Stress Test Suite
Comment=Professional hardware testing tool
Exec=/usr/local/bin/stress_test
Icon=utilities-system-monitor
Terminal=true
Categories=System;Monitor;
EOF

    chown $ACTUAL_USER:$ACTUAL_USER $DESKTOP_DIR/stress-test.desktop
    chmod 755 $DESKTOP_DIR/stress-test.desktop
    
    echo "✓ Desktop shortcut created"
fi

echo ""
echo "=========================================="
echo " Installation Complete! ✓"
echo "=========================================="
echo ""
echo "Usage:"
echo "  stress_test          # Run on default port (8080)"
echo "  stress_test 9000     # Run on custom port"
echo ""
echo "Then open your browser and go to:"
echo "  http://localhost:8080"
echo ""
echo "Uninstall with:"
echo "  sudo ./uninstall.sh"
echo ""
echo "Documentation:"
echo "  README.md        # Full documentation"
echo "  QUICKSTART.md    # Quick start guide"
echo "  CONTRIBUTING.md  # Contribution guidelines"
echo ""
