#!/bin/bash
# Build script for Hardware Stress Test Suite
# MIT License (c) 2026 kj-devvixon

echo "=========================================="
echo " Hardware Stress Test Suite - Build"
echo " MIT License © 2026 kj-devvixon"
echo "=========================================="
echo ""

# Check for compiler
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler not found!"
    echo "Install with: sudo apt-get install build-essential"
    exit 1
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Choose build system
echo ""
echo "Select build system:"
echo "1) CMake (recommended)"
echo "2) Make (direct)"
read -p "Enter choice [1-2]: " choice

case $choice in
    1)
        if ! command -v cmake &> /dev/null; then
            echo "Error: CMake not found!"
            echo "Install with: sudo apt-get install cmake"
            exit 1
        fi
        
        echo ""
        echo "Building with CMake..."
        cmake ..
        make
        
        if [ $? -eq 0 ]; then
            echo ""
            echo "✓ Build successful!"
            echo "Executable: ./build/stress_test"
            echo ""
            read -p "Run now? [y/N]: " run
            if [ "$run" = "y" ] || [ "$run" = "Y" ]; then
                ./stress_test
            fi
        else
            echo "✗ Build failed!"
            exit 1
        fi
        ;;
    2)
        cd ..
        echo ""
        echo "Building with Make..."
        make
        
        if [ $? -eq 0 ]; then
            echo ""
            echo "✓ Build successful!"
            echo "Executable: ./stress_test"
            echo ""
            read -p "Run now? [y/N]: " run
            if [ "$run" = "y" ] || [ "$run" = "Y" ]; then
                ./stress_test
            fi
        else
            echo "✗ Build failed!"
            exit 1
        fi
        ;;
    *)
        echo "Invalid choice!"
        exit 1
        ;;
esac
