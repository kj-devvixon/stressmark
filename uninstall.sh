#!/bin/bash
# Uninstall script for Hardware Stress Test Suite
# MIT License (c) 2026 kj-devvixon

echo "=========================================="
echo " Hardware Stress Test Suite - Uninstaller"
echo " MIT License © 2026 kj-devvixon"
echo "=========================================="
echo ""

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    echo "This script requires root privileges."
    echo "Please run with sudo: sudo ./uninstall.sh"
    exit 1
fi

# Get the actual user
ACTUAL_USER="${SUDO_USER:-$USER}"
ACTUAL_HOME=$(eval echo ~$ACTUAL_USER)

echo "Uninstalling..."
echo ""

# Stop and disable service if exists
if systemctl is-active --quiet stress-test.service; then
    echo "Stopping stress-test service..."
    systemctl stop stress-test.service
    echo "✓ Service stopped"
fi

if systemctl is-enabled --quiet stress-test.service 2>/dev/null; then
    echo "Disabling stress-test service..."
    systemctl disable stress-test.service
    echo "✓ Service disabled"
fi

if [ -f "/etc/systemd/system/stress-test.service" ]; then
    echo "Removing service file..."
    rm /etc/systemd/system/stress-test.service
    systemctl daemon-reload
    echo "✓ Service file removed"
fi

# Remove binary
if [ -f "/usr/local/bin/stress_test" ]; then
    echo "Removing binary..."
    rm /usr/local/bin/stress_test
    echo "✓ Binary removed"
fi

# Remove desktop entry
DESKTOP_FILE="$ACTUAL_HOME/.local/share/applications/stress-test.desktop"
if [ -f "$DESKTOP_FILE" ]; then
    echo "Removing desktop shortcut..."
    rm "$DESKTOP_FILE"
    echo "✓ Desktop shortcut removed"
fi

echo ""
echo "=========================================="
echo " Uninstallation Complete! ✓"
echo "=========================================="
echo ""
echo "The source code in this directory has not been removed."
echo "You can delete it manually if you wish."
echo ""
