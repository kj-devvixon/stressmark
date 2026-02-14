# Changelog

All notable changes to the Hardware Stress Test Suite will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-02-14

### Added
- Initial release of Hardware Stress Test Suite
- CPU Single-Core stress test
- CPU Multi-Core stress test
- RAM stress test with memory allocation and sorting
- GPU stress test (basic implementation)
- Combined stress test (all components simultaneously)
- Embedded HTTP server for web interface
- Modern, responsive HTML/CSS interface
- Real-time statistics monitoring:
  - CPU usage percentage
  - RAM usage percentage
  - RAM used in MB
  - CPU temperature (Linux)
- System information detection:
  - CPU model and specifications
  - CPU cores and threads count
  - CPU frequency
  - Total and available RAM
  - GPU information
- CMake build system
- Makefile for direct compilation
- Build scripts for Linux and Windows
- Installation and uninstallation scripts
- Comprehensive documentation:
  - README.md with full documentation
  - QUICKSTART.md for quick start guide
  - CONTRIBUTING.md for contributors
- MIT License
- .gitignore for version control
- Configuration file example (config.ini)

### Technical Details
- C++17 standard
- Multi-threaded test execution
- Cross-platform design (Linux/Windows)
- REST API for frontend-backend communication
- No external dependencies except standard libraries and pthread
- Optimized with -O3 -march=native compilation flags

### API Endpoints
- `GET /` - Main web interface
- `GET /api/system-info` - System information (JSON)
- `GET /api/start-test` - Start stress test
- `GET /api/stop-test` - Stop running test
- `GET /api/test-status` - Get current test status (JSON)

### Known Issues
- GPU test is CPU-based simulation (no OpenGL/Vulkan yet)
- Temperature reading requires permissions on Linux
- Windows implementation incomplete (basic functionality only)

### Planned for Next Release
- Full Windows support with PDH library integration
- OpenGL/Vulkan GPU stress test
- Real-time graph rendering
- Export test results to CSV/JSON
- Dark mode for UI
- Configuration file loading
- Command-line arguments parsing
- More detailed error messages

---

## [Unreleased]

### Planned Features
- [ ] Advanced GPU stress testing with OpenGL/Vulkan
- [ ] Historical data graphs with Chart.js
- [ ] Test result export (CSV, JSON, PDF)
- [ ] Dark mode theme
- [ ] Multiple language support
- [ ] Mobile-responsive improvements
- [ ] Sound notifications
- [ ] Test profiles and presets
- [ ] Benchmark mode with scoring system
- [ ] Comparison with reference systems
- [ ] Automated test scheduling
- [ ] REST API documentation
- [ ] WebSocket for real-time updates
- [ ] Docker container support
- [ ] Snap/Flatpak packages
- [ ] Windows MSI installer
- [ ] macOS support
- [ ] ARM architecture support
- [ ] Power consumption monitoring
- [ ] Network stress test
- [ ] Disk I/O stress test

### Improvements Backlog
- Unit tests
- Integration tests
- CI/CD pipeline (GitHub Actions)
- Code coverage reports
- Performance profiling
- Memory leak detection
- Better error handling
- Logging system
- Plugin architecture
- Community contributed test profiles

---

## Version History Format

### [X.Y.Z] - YYYY-MM-DD
- X = Major version (breaking changes)
- Y = Minor version (new features, backwards compatible)
- Z = Patch version (bug fixes)

### Categories
- **Added** - New features
- **Changed** - Changes to existing functionality
- **Deprecated** - Features that will be removed
- **Removed** - Removed features
- **Fixed** - Bug fixes
- **Security** - Security fixes

---

**MIT License © 2026 kj-devvixon**
