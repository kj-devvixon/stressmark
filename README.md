# StressMark

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/C++-17-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows-lightgrey.svg)

A professional hardware stress testing tool similar to FurMark, featuring a modern HTML/CSS web interface with C++ backend.

**Author:** kj-devvixon  
**License:** MIT  
**Version:** 1.0.0

## 🌟 Features

### Comprehensive Stress Tests
- **CPU Single-Core Test** - Tests single-thread performance with intensive mathematical operations
- **CPU Multi-Core Test** - Stresses all CPU cores simultaneously
- **RAM Stress Test** - Memory allocation, sorting, and access patterns (similar to memtest)
- **GPU Stress Test** - Graphics processing stress test
- **Combined Test** - All components tested simultaneously

### Modern Web Interface
- Beautiful, responsive HTML/CSS interface
- Real-time statistics monitoring
- Live CPU usage, RAM usage, and temperature graphs
- Progress indicators
- Easy-to-use controls

### System Monitoring
- CPU model, cores, threads, frequency detection
- RAM capacity and availability
- GPU information
- Real-time temperature monitoring (Linux)
- Usage statistics collection

## 📋 Requirements

### Build Requirements
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+
- pthread library (usually pre-installed on Linux)

### Runtime Requirements
- Modern web browser (Chrome, Firefox, Edge, Safari)
- Linux or Windows OS
- Sufficient system resources for stress testing

## 🚀 Building the Project

### Linux

```bash
# Clone or download the project
cd hardware-stress-test

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make

# Run
./stress_test
```

### Alternative Build (without CMake)

```bash
# Direct compilation
g++ -std=c++17 -O3 -march=native -pthread \
    -I./include \
    src/main.cpp src/stress_test.cpp \
    -o stress_test
```

### Windows (Visual Studio)

```batch
mkdir build
cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
```

Or use Visual Studio to open the CMakeLists.txt directly.

## 📖 Usage

### Starting the Application

```bash
# Default port (8080)
./stress_test

# Custom port
./stress_test 9000
```

### Opening the Web Interface

Once the application is running, open your web browser and navigate to:

```
http://localhost:8080
```

### Running Tests

1. The application will automatically display your system information
2. Select one of the five test types:
   - CPU Single-Core
   - CPU Multi-Core
   - RAM Stress Test
   - GPU Stress Test
   - Combined Test
3. Set the test duration (in seconds)
4. Click "Start Test"
5. Monitor real-time statistics:
   - CPU Usage (%)
   - RAM Usage (%)
   - RAM Used (MB)
   - CPU Temperature (°C)
6. Click "Stop Test" to end the test early (if needed)

### Safety Recommendations

⚠️ **Important Safety Notes:**
- Start with shorter durations (30-60 seconds) to ensure system stability
- Monitor temperatures closely - stop if temperatures exceed safe limits
- Ensure adequate cooling before running extended tests
- Save all work before running stress tests
- Combined tests are the most intensive - use with caution

## 🏗️ Project Structure

```
hardware-stress-test/
├── LICENSE                 # MIT License
├── README.md              # This file
├── CMakeLists.txt         # CMake build configuration
├── include/
│   └── stress_test.h      # Header file with class declarations
├── src/
│   ├── main.cpp           # Main application with HTTP server
│   └── stress_test.cpp    # Stress test implementations
└── build/                 # Build directory (created during compilation)
```

## 🎯 Technical Details

### Architecture

The application consists of two main components:

1. **C++ Backend**
   - Implements all stress test algorithms
   - System information gathering
   - Embedded HTTP server for API endpoints
   - Multi-threaded test execution
   - Real-time statistics collection

2. **HTML/CSS Frontend**
   - Single-page application
   - Modern, responsive design
   - Real-time data updates via REST API
   - No external dependencies

### API Endpoints

- `GET /` - Serve the main HTML interface
- `GET /api/system-info` - Get system information (JSON)
- `GET /api/start-test?type=<test_type>&duration=<seconds>` - Start a test
- `GET /api/stop-test` - Stop the current test
- `GET /api/test-status` - Get current test status and statistics (JSON)

### Stress Test Algorithms

**CPU Tests:**
- Mathematical operations: sqrt, sin, cos, pow, log
- Continuous loops to maximize CPU utilization
- Single-core: runs on one thread
- Multi-core: spawns thread per CPU core

**RAM Test:**
- Allocates 50MB blocks dynamically
- Fills memory with random data
- Performs sorting operations
- Tests memory access patterns
- Maintains memory pressure

**GPU Test:**
- Simulates graphics calculations
- Pixel processing operations
- Continuous rendering loop

## 🔧 Customization

### Changing Test Parameters

Edit `stress_test.cpp` to modify test behavior:

```cpp
// Change block size in RAM test (line ~XXX)
const size_t blockSize = 50 * 1024 * 1024 / sizeof(double);

// Change iteration count in CPU tests (line ~XXX)
for (int i = 0; i < 100000 && running; ++i) {
```

### Modifying the Interface

Edit the HTML in `src/main.cpp` function `getHTMLPage()`:
- Change colors in the `<style>` section
- Modify layout in the `<body>` section
- Add new features in the `<script>` section

## 🐛 Troubleshooting

### Port Already in Use

```bash
# Use a different port
./stress_test 9000
```

### Temperature Reading Not Working

Temperature reading requires access to `/sys/class/thermal/` on Linux. Run with appropriate permissions or check if thermal zones are available:

```bash
ls /sys/class/thermal/
```

### Build Errors

Ensure you have all required dependencies:

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake

# Fedora/RHEL
sudo dnf install gcc-c++ cmake make
```

## 📝 License

MIT License

Copyright (c) 2026 kj-devvixon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## 🤝 Contributing

Contributions are welcome! Feel free to:
- Report bugs
- Suggest new features
- Submit pull requests
- Improve documentation

## 📞 Contact

Created by **kj-devvixon**

---

**Note:** This stress testing tool can put significant load on your hardware. Use responsibly and ensure your system has adequate cooling. The authors are not responsible for any hardware damage that may occur from misuse.
