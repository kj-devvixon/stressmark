#include "stress_test.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
#elif __linux__
#include <sys/sysinfo.h>
#include <unistd.h>
#endif

namespace StressTest {

// ============================================================================
// StressTestBase Implementation
// ============================================================================

StressTestBase::StressTestBase(int durationSeconds) 
    : running(false), completed(false), duration(durationSeconds) {}

StressTestBase::~StressTestBase() {
    stop();
}

void StressTestBase::stop() {
    running = false;
}

bool StressTestBase::isRunning() const {
    return running;
}

bool StressTestBase::isCompleted() const {
    return completed;
}

std::vector<TestStats> StressTestBase::getStats() const {
    return stats;
}

void StressTestBase::setStatsCallback(std::function<void(const TestStats&)> callback) {
    statsCallback = callback;
}

void StressTestBase::collectStats() {
    while (running) {
        TestStats stat;
        stat.cpuUsage = getCPUUsage();
        stat.ramUsage = getRAMUsage();
        stat.ramUsedMB = getRAMUsedMB();
        stat.cpuTemp = getCPUTemperature();
        stat.timestamp = std::chrono::steady_clock::now();
        
        stats.push_back(stat);
        
        if (statsCallback) {
            statsCallback(stat);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

double StressTestBase::getCPUUsage() {
#ifdef __linux__
    static unsigned long long lastTotal = 0;
    static unsigned long long lastIdle = 0;
    
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        return 0.0;
    }
    
    std::string line;
    std::getline(file, line);
    
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    std::istringstream ss(line);
    std::string cpu;
    ss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    
    unsigned long long totalIdle = idle + iowait;
    unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
    
    if (lastTotal == 0) {
        lastTotal = total;
        lastIdle = totalIdle;
        return 0.0;
    }
    
    unsigned long long totalDelta = total - lastTotal;
    unsigned long long idleDelta = totalIdle - lastIdle;
    
    lastTotal = total;
    lastIdle = totalIdle;
    
    if (totalDelta == 0) {
        return 0.0;
    }
    
    return ((double)(totalDelta - idleDelta) / totalDelta) * 100.0;
#else
    return 0.0; // Windows implementation would use PDH
#endif
}

double StressTestBase::getRAMUsage() {
#ifdef __linux__
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        unsigned long long totalRAM = info.totalram;
        unsigned long long freeRAM = info.freeram;
        return ((totalRAM - freeRAM) * 100.0) / totalRAM;
    }
#endif
    return 0.0;
}

unsigned long long StressTestBase::getRAMUsedMB() {
#ifdef __linux__
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        unsigned long long usedRAM = info.totalram - info.freeram;
        return (usedRAM * info.mem_unit) / (1024 * 1024);
    }
#endif
    return 0;
}

double StressTestBase::getCPUTemperature() {
#ifdef __linux__
    // Method 1: Try hwmon (most reliable on modern systems)
    // Look for coretemp, k10temp, etc.
    std::vector<std::string> hwmonPaths = {
        "/sys/class/hwmon/hwmon0/temp1_input",
        "/sys/class/hwmon/hwmon1/temp1_input",
        "/sys/class/hwmon/hwmon2/temp1_input",
        "/sys/class/hwmon/hwmon3/temp1_input",
        "/sys/class/hwmon/hwmon4/temp1_input",
        "/sys/class/hwmon/hwmon5/temp1_input"
    };
    
    for (const auto& path : hwmonPaths) {
        std::ifstream tempFile(path);
        if (tempFile.is_open()) {
            int temp;
            tempFile >> temp;
            if (temp > 0 && temp < 150000) { // Sanity check: 0-150°C
                return temp / 1000.0;
            }
        }
    }
    
    // Method 2: Try thermal zone
    std::vector<std::string> thermalPaths = {
        "/sys/class/thermal/thermal_zone0/temp",
        "/sys/class/thermal/thermal_zone1/temp",
        "/sys/class/thermal/thermal_zone2/temp"
    };
    
    for (const auto& path : thermalPaths) {
        std::ifstream tempFile(path);
        if (tempFile.is_open()) {
            int temp;
            tempFile >> temp;
            if (temp > 0 && temp < 150000) {
                return temp / 1000.0;
            }
        }
    }
    
    // Method 3: Try using sensors command
    FILE* pipe = popen("sensors 2>/dev/null | grep -i 'Package id 0\\|Tctl\\|Tdie\\|Core 0' | head -1 | grep -oP '\\+\\K[0-9]+' | head -1", "r");
    if (pipe) {
        char buffer[128];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            double temp = std::atof(buffer);
            pclose(pipe);
            if (temp > 0 && temp < 150) {
                return temp;
            }
        }
        pclose(pipe);
    }
#endif
    return 0.0;
}

// ============================================================================
// CPUSingleCoreTest Implementation
// ============================================================================

CPUSingleCoreTest::CPUSingleCoreTest(int durationSeconds) 
    : StressTestBase(durationSeconds) {}

CPUSingleCoreTest::~CPUSingleCoreTest() {
    stop();
    if (testThread.joinable()) testThread.join();
    if (statsThread.joinable()) statsThread.join();
}

void CPUSingleCoreTest::start() {
    if (running) return;
    
    running = true;
    completed = false;
    stats.clear();
    
    testThread = std::thread(&CPUSingleCoreTest::runTest, this);
    statsThread = std::thread(&CPUSingleCoreTest::collectStats, this);
}

void CPUSingleCoreTest::stop() {
    StressTestBase::stop();
    if (testThread.joinable()) testThread.join();
    if (statsThread.joinable()) statsThread.join();
}

void CPUSingleCoreTest::runTest() {
    auto endTime = std::chrono::steady_clock::now() + 
                   std::chrono::seconds(duration);
    
    cpuIntensiveWork();
    
    while (running && std::chrono::steady_clock::now() < endTime) {
        cpuIntensiveWork();
    }
    
    running = false;
    completed = true;
}

void CPUSingleCoreTest::cpuIntensiveWork() {
    volatile double result = 0.0;
    
    // Mathematical operations to stress CPU
    for (int i = 0; i < 100000 && running; ++i) {
        result += std::sqrt(i);
        result = std::sin(result) * std::cos(result);
        result = std::pow(result, 1.5);
        result = std::log(std::abs(result) + 1.0);
    }
}

// ============================================================================
// CPUMultiCoreTest Implementation
// ============================================================================

CPUMultiCoreTest::CPUMultiCoreTest(int durationSeconds) 
    : StressTestBase(durationSeconds) {}

CPUMultiCoreTest::~CPUMultiCoreTest() {
    stop();
    for (auto& thread : testThreads) {
        if (thread.joinable()) thread.join();
    }
    if (statsThread.joinable()) statsThread.join();
}

void CPUMultiCoreTest::start() {
    if (running) return;
    
    running = true;
    completed = false;
    stats.clear();
    
    unsigned int numThreads = std::thread::hardware_concurrency();
    testThreads.clear();
    
    for (unsigned int i = 0; i < numThreads; ++i) {
        testThreads.emplace_back(&CPUMultiCoreTest::runTest, this);
    }
    
    statsThread = std::thread(&CPUMultiCoreTest::collectStats, this);
}

void CPUMultiCoreTest::stop() {
    StressTestBase::stop();
    for (auto& thread : testThreads) {
        if (thread.joinable()) thread.join();
    }
    if (statsThread.joinable()) statsThread.join();
}

void CPUMultiCoreTest::runTest() {
    auto endTime = std::chrono::steady_clock::now() + 
                   std::chrono::seconds(duration);
    
    while (running && std::chrono::steady_clock::now() < endTime) {
        cpuIntensiveWork();
    }
}

void CPUMultiCoreTest::cpuIntensiveWork() {
    volatile double result = 0.0;
    
    for (int i = 0; i < 100000 && running; ++i) {
        result += std::sqrt(i);
        result = std::sin(result) * std::cos(result);
        result = std::pow(result, 1.5);
        result = std::log(std::abs(result) + 1.0);
    }
}

// ============================================================================
// RAMStressTest Implementation
// ============================================================================

RAMStressTest::RAMStressTest(int durationSeconds) 
    : StressTestBase(durationSeconds) {}

RAMStressTest::~RAMStressTest() {
    stop();
    if (testThread.joinable()) testThread.join();
    if (statsThread.joinable()) statsThread.join();
}

void RAMStressTest::start() {
    if (running) return;
    
    running = true;
    completed = false;
    stats.clear();
    memoryBlocks.clear();
    
    testThread = std::thread(&RAMStressTest::runTest, this);
    statsThread = std::thread(&RAMStressTest::collectStats, this);
}

void RAMStressTest::stop() {
    StressTestBase::stop();
    if (testThread.joinable()) testThread.join();
    if (statsThread.joinable()) statsThread.join();
    memoryBlocks.clear();
}

void RAMStressTest::runTest() {
    auto endTime = std::chrono::steady_clock::now() + 
                   std::chrono::seconds(duration);
    
    while (running && std::chrono::steady_clock::now() < endTime) {
        allocateAndStress();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    running = false;
    completed = true;
    memoryBlocks.clear();
}

void RAMStressTest::allocateAndStress() {
    try {
        // Allocate 50MB blocks
        const size_t blockSize = 50 * 1024 * 1024 / sizeof(double);
        std::vector<double> block(blockSize);
        
        // Fill with random data
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        for (size_t i = 0; i < blockSize && running; i += 1000) {
            block[i] = dis(gen);
        }
        
        // Sort to stress RAM access patterns
        std::sort(block.begin(), block.end());
        
        // Calculate some statistics
        double sum = std::accumulate(block.begin(), block.end(), 0.0);
        volatile double result = sum / blockSize;
        
        memoryBlocks.push_back(std::move(block));
        
        // Keep only last 10 blocks to avoid OOM
        if (memoryBlocks.size() > 10) {
            memoryBlocks.erase(memoryBlocks.begin());
        }
    } catch (const std::bad_alloc&) {
        // Memory allocation failed, clear some blocks
        if (!memoryBlocks.empty()) {
            memoryBlocks.erase(memoryBlocks.begin());
        }
    }
}

// ============================================================================
// GPUStressTest Implementation
// ============================================================================

GPUStressTest::GPUStressTest(int durationSeconds) 
    : StressTestBase(durationSeconds) {}

GPUStressTest::~GPUStressTest() {
    stop();
    if (testThread.joinable()) testThread.join();
    if (statsThread.joinable()) statsThread.join();
}

void GPUStressTest::start() {
    if (running) return;
    
    running = true;
    completed = false;
    stats.clear();
    
    testThread = std::thread(&GPUStressTest::runTest, this);
    statsThread = std::thread(&GPUStressTest::collectStats, this);
}

void GPUStressTest::stop() {
    StressTestBase::stop();
    if (testThread.joinable()) testThread.join();
    if (statsThread.joinable()) statsThread.join();
}

void GPUStressTest::runTest() {
    auto endTime = std::chrono::steady_clock::now() + 
                   std::chrono::seconds(duration);
    
    // Note: Full GPU testing would require OpenGL/Vulkan/CUDA
    // This is a placeholder that does CPU-based graphics calculations
    while (running && std::chrono::steady_clock::now() < endTime) {
        gpuIntensiveWork();
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    running = false;
    completed = true;
}

void GPUStressTest::gpuIntensiveWork() {
    // Simulate graphics calculations
    const int width = 1920;
    const int height = 1080;
    std::vector<double> pixels(width * height * 3);
    
    for (int y = 0; y < height && running; y += 10) {
        for (int x = 0; x < width; x += 10) {
            int idx = (y * width + x) * 3;
            double r = std::sin(x * 0.01) * 0.5 + 0.5;
            double g = std::cos(y * 0.01) * 0.5 + 0.5;
            double b = std::sin((x + y) * 0.005) * 0.5 + 0.5;
            pixels[idx] = r;
            pixels[idx + 1] = g;
            pixels[idx + 2] = b;
        }
    }
}

// ============================================================================
// CombinedStressTest Implementation
// ============================================================================

CombinedStressTest::CombinedStressTest(int durationSeconds) 
    : StressTestBase(durationSeconds) {}

CombinedStressTest::~CombinedStressTest() {
    stop();
    if (statsThread.joinable()) statsThread.join();
}

void CombinedStressTest::start() {
    if (running) return;
    
    running = true;
    completed = false;
    stats.clear();
    
    // Start all tests simultaneously
    cpuTest = std::make_unique<CPUMultiCoreTest>(duration);
    ramTest = std::make_unique<RAMStressTest>(duration);
    gpuTest = std::make_unique<GPUStressTest>(duration);
    
    cpuTest->start();
    ramTest->start();
    gpuTest->start();
    
    statsThread = std::thread(&CombinedStressTest::collectStats, this);
    
    // Wait for tests to complete
    std::thread([this]() {
        auto endTime = std::chrono::steady_clock::now() + 
                       std::chrono::seconds(duration);
        
        while (running && std::chrono::steady_clock::now() < endTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        running = false;
        completed = true;
        
        if (cpuTest) cpuTest->stop();
        if (ramTest) ramTest->stop();
        if (gpuTest) gpuTest->stop();
    }).detach();
}

void CombinedStressTest::stop() {
    StressTestBase::stop();
    if (cpuTest) cpuTest->stop();
    if (ramTest) ramTest->stop();
    if (gpuTest) gpuTest->stop();
    if (statsThread.joinable()) statsThread.join();
}

// ============================================================================
// SystemInfoGatherer Implementation
// ============================================================================

SystemInfo SystemInfoGatherer::getSystemInfo() {
    SystemInfo info;
    info.cpuModel = getCPUModel();
    info.cpuCores = getCPUCores();
    info.cpuThreads = getCPUThreads();
    info.cpuFrequency = getCPUFrequency();
    info.totalRAM = getTotalRAM();
    info.availableRAM = getAvailableRAM();
    info.gpuInfo = getGPUInfo();
    return info;
}

std::string SystemInfoGatherer::getCPUModel() {
#ifdef __linux__
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("model name") != std::string::npos) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                return line.substr(pos + 2);
            }
        }
    }
#endif
    return "Unknown CPU";
}

int SystemInfoGatherer::getCPUCores() {
    return std::thread::hardware_concurrency() / 2; // Assuming hyperthreading
}

int SystemInfoGatherer::getCPUThreads() {
    return std::thread::hardware_concurrency();
}

double SystemInfoGatherer::getCPUFrequency() {
#ifdef __linux__
    std::ifstream freqFile("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
    if (freqFile.is_open()) {
        long freq;
        freqFile >> freq;
        return freq / 1000.0; // Convert to MHz
    }
#endif
    return 0.0;
}

unsigned long long SystemInfoGatherer::getTotalRAM() {
#ifdef __linux__
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return (info.totalram * info.mem_unit) / (1024 * 1024); // MB
    }
#endif
    return 0;
}

unsigned long long SystemInfoGatherer::getAvailableRAM() {
#ifdef __linux__
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return (info.freeram * info.mem_unit) / (1024 * 1024); // MB
    }
#endif
    return 0;
}

std::string SystemInfoGatherer::getGPUInfo() {
#ifdef __linux__
    // Try to get GPU info from lspci
    FILE* pipe = popen("lspci | grep -i vga", "r");
    if (pipe) {
        char buffer[256];
        std::string result;
        while (fgets(buffer, sizeof(buffer), pipe)) {
            result += buffer;
        }
        pclose(pipe);
        if (!result.empty()) {
            return result;
        }
    }
#endif
    return "Unknown GPU";
}

} // namespace StressTest
