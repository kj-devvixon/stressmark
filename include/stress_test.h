#ifndef STRESS_TEST_H
#define STRESS_TEST_H

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

namespace StressTest {

// System information structure
struct SystemInfo {
    std::string cpuModel;
    int cpuCores;
    int cpuThreads;
    double cpuFrequency;
    unsigned long long totalRAM;
    unsigned long long availableRAM;
    std::string gpuInfo;
};

// Test statistics
struct TestStats {
    double cpuUsage;
    double ramUsage;
    double cpuTemp;
    unsigned long long ramUsedMB;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
};

// Base class for stress tests
class StressTestBase {
protected:
    std::atomic<bool> running;
    std::atomic<bool> completed;
    int duration;
    std::vector<TestStats> stats;
    std::function<void(const TestStats&)> statsCallback;
    
public:
    StressTestBase(int durationSeconds);
    virtual ~StressTestBase();
    
    virtual void start() = 0;
    virtual void stop();
    virtual bool isRunning() const;
    virtual bool isCompleted() const;
    virtual std::vector<TestStats> getStats() const;
    
    void setStatsCallback(std::function<void(const TestStats&)> callback);
    
protected:
    void collectStats();
    double getCPUUsage();
    double getRAMUsage();
    unsigned long long getRAMUsedMB();
    double getCPUTemperature();
};

// Single-core CPU test
class CPUSingleCoreTest : public StressTestBase {
private:
    std::thread testThread;
    std::thread statsThread;
    
    void runTest();
    void cpuIntensiveWork();
    
public:
    CPUSingleCoreTest(int durationSeconds);
    ~CPUSingleCoreTest();
    
    void start() override;
    void stop() override;
};

// Multi-core CPU test
class CPUMultiCoreTest : public StressTestBase {
private:
    std::vector<std::thread> testThreads;
    std::thread statsThread;
    
    void runTest();
    void cpuIntensiveWork();
    
public:
    CPUMultiCoreTest(int durationSeconds);
    ~CPUMultiCoreTest();
    
    void start() override;
    void stop() override;
};

// RAM stress test
class RAMStressTest : public StressTestBase {
private:
    std::thread testThread;
    std::thread statsThread;
    std::vector<std::vector<double>> memoryBlocks;
    
    void runTest();
    void allocateAndStress();
    
public:
    RAMStressTest(int durationSeconds);
    ~RAMStressTest();
    
    void start() override;
    void stop() override;
};

// GPU stress test
class GPUStressTest : public StressTestBase {
private:
    std::thread testThread;
    std::thread statsThread;
    
    void runTest();
    void gpuIntensiveWork();
    
public:
    GPUStressTest(int durationSeconds);
    ~GPUStressTest();
    
    void start() override;
    void stop() override;
};

// Combined test (all components)
class CombinedStressTest : public StressTestBase {
private:
    std::unique_ptr<CPUMultiCoreTest> cpuTest;
    std::unique_ptr<RAMStressTest> ramTest;
    std::unique_ptr<GPUStressTest> gpuTest;
    std::thread statsThread;
    
public:
    CombinedStressTest(int durationSeconds);
    ~CombinedStressTest();
    
    void start() override;
    void stop() override;
};

// System information gatherer
class SystemInfoGatherer {
public:
    static SystemInfo getSystemInfo();
    
private:
    static std::string getCPUModel();
    static int getCPUCores();
    static int getCPUThreads();
    static double getCPUFrequency();
    static unsigned long long getTotalRAM();
    static unsigned long long getAvailableRAM();
    static std::string getGPUInfo();
};

} // namespace StressTest

#endif // STRESS_TEST_H
