# Usage Examples

This file contains practical examples of using the Hardware Stress Test Suite.

## Table of Contents
1. [Basic Usage](#basic-usage)
2. [Testing Scenarios](#testing-scenarios)
3. [Automation Examples](#automation-examples)
4. [Integration Examples](#integration-examples)
5. [Troubleshooting Examples](#troubleshooting-examples)

---

## Basic Usage

### Running a Quick Test

```bash
# Start the application
./stress_test

# In browser: http://localhost:8080
# Select "CPU Single-Core"
# Duration: 30 seconds
# Click "Start Test"
```

### Running on Custom Port

```bash
# Use port 9000
./stress_test 9000

# Access at http://localhost:9000
```

### Quick CLI Test

```bash
# Start server in background
./stress_test &
SERVER_PID=$!

# Wait for server to start
sleep 2

# Start test via API
curl "http://localhost:8080/api/start-test?type=cpu_multi&duration=60"

# Monitor test
while true; do
    status=$(curl -s http://localhost:8080/api/test-status)
    running=$(echo $status | jq -r '.running')
    if [ "$running" = "false" ]; then
        break
    fi
    echo $status | jq '{cpuUsage, ramUsage, cpuTemp}'
    sleep 1
done

# Stop server
kill $SERVER_PID
```

---

## Testing Scenarios

### Scenario 1: New PC Build Testing

Test a newly built PC to ensure stability:

```bash
# 1. Quick sanity check (5 minutes)
# Single-core test: 5 minutes
# Multi-core test: 5 minutes
# RAM test: 5 minutes

# 2. Medium stability test (30 minutes)
# Combined test: 30 minutes
# Monitor temperatures closely

# 3. Extended stress test (2+ hours)
# Multi-core test: 2 hours
# RAM test: 2 hours
# Combined test: 1 hour

# 4. Verify
# - No crashes or freezes
# - Temperatures stay below 85°C
# - No thermal throttling
# - System remains responsive
```

### Scenario 2: Overclocking Validation

Test system stability after overclocking:

```bash
# 1. Baseline test BEFORE overclock
./stress_test
# Run combined test for 30 minutes
# Note down: Max temp, CPU usage, stability

# 2. Apply overclock in BIOS

# 3. Incremental testing
# Start with: CPU Single-Core, 5 minutes
# If stable: CPU Multi-Core, 10 minutes  
# If stable: Combined test, 30 minutes
# If stable: Extended test, 2+ hours

# 4. If unstable
# - Reduce overclock
# - Increase voltage slightly
# - Improve cooling
# - Repeat testing
```

### Scenario 3: Laptop Cooling Test

Test laptop cooling under sustained load:

```bash
# Ensure laptop is on hard, flat surface
# Clean fans/vents if possible

# 1. Initial test
./stress_test
# CPU Multi-Core, 10 minutes
# Monitor: Does temperature plateau or keep rising?

# 2. Thermal throttling check
# If temp reaches 95°C+ → Thermal throttling likely
# If temp stabilizes 70-85°C → Cooling adequate
# If temp stays below 70°C → Excellent cooling

# 3. Sustained load
# Combined test, 30 minutes
# Listen for fan speed changes
# Check if performance degrades over time
```

### Scenario 4: RAM Stability Testing

Test RAM for errors and stability:

```bash
# Similar to memtest86 but in OS

# 1. Quick RAM test
./stress_test
# RAM test, 15 minutes

# 2. Extended RAM test  
# RAM test, 2+ hours
# or overnight for thorough testing

# 3. What to look for:
# - System crashes → Bad RAM
# - Random freezes → RAM timing issues
# - Application errors → Memory instability
# - No issues → RAM is stable

# 4. If issues found:
# - Test one stick at a time
# - Try different RAM slots
# - Reduce RAM speed in BIOS
# - Increase RAM voltage slightly
```

### Scenario 5: Server Stress Testing

Test server before production deployment:

```bash
# 1. Component testing
# CPU: Multi-core, 1 hour
# RAM: RAM test, 2 hours

# 2. Combined load
# Combined test, 4 hours
# Simulates heavy production load

# 3. Thermal management
# Ensure temperatures are acceptable under load
# Check if thermal throttling occurs
# Verify cooling solution is adequate

# 4. Stability criteria
# Must complete tests without:
# - Crashes
# - Freezes
# - Errors
# - Thermal throttling
```

---

## Automation Examples

### Automated Daily Testing

```bash
#!/bin/bash
# daily-stress-test.sh

LOG_FILE="/var/log/stress-test-$(date +%Y%m%d).log"

echo "Starting daily stress test - $(date)" | tee -a $LOG_FILE

# Start server
./stress_test 8080 &
SERVER_PID=$!
sleep 3

# Run test
curl -s "http://localhost:8080/api/start-test?type=combined&duration=3600" | tee -a $LOG_FILE

# Monitor for 1 hour
for i in {1..360}; do
    status=$(curl -s http://localhost:8080/api/test-status)
    echo "$(date +%H:%M:%S) - $status" | tee -a $LOG_FILE
    sleep 10
done

# Stop server
kill $SERVER_PID

echo "Test completed - $(date)" | tee -a $LOG_FILE
```

### Temperature Monitoring Script

```bash
#!/bin/bash
# temp-monitor.sh

MAX_TEMP=85
ALERT_EMAIL="admin@example.com"

while true; do
    status=$(curl -s http://localhost:8080/api/test-status)
    temp=$(echo $status | jq -r '.cpuTemp')
    
    if (( $(echo "$temp > $MAX_TEMP" | bc -l) )); then
        echo "ALERT: Temperature ${temp}°C exceeds ${MAX_TEMP}°C" | \
            mail -s "Temperature Alert" $ALERT_EMAIL
        
        # Stop test
        curl -s http://localhost:8080/api/stop-test
        break
    fi
    
    sleep 5
done
```

### Benchmark Script

```bash
#!/bin/bash
# benchmark.sh

echo "Running benchmark suite..."

# Test configurations
TESTS=("cpu_single" "cpu_multi" "ram_test")
DURATION=300  # 5 minutes each

# Start server
./stress_test &
SERVER_PID=$!
sleep 2

for test in "${TESTS[@]}"; do
    echo "Running $test test..."
    
    # Start test
    curl -s "http://localhost:8080/api/start-test?type=$test&duration=$DURATION"
    
    # Collect data
    max_cpu=0
    max_temp=0
    
    while true; do
        status=$(curl -s http://localhost:8080/api/test-status)
        running=$(echo $status | jq -r '.running')
        
        if [ "$running" = "false" ]; then
            break
        fi
        
        cpu=$(echo $status | jq -r '.cpuUsage')
        temp=$(echo $status | jq -r '.cpuTemp')
        
        # Track maximums
        max_cpu=$(echo "$cpu $max_cpu" | awk '{print ($1>$2)?$1:$2}')
        max_temp=$(echo "$temp $max_temp" | awk '{print ($1>$2)?$1:$2}')
        
        sleep 1
    done
    
    echo "$test results: Max CPU: $max_cpu%, Max Temp: $max_temp°C"
    echo ""
    
    sleep 10  # Cool down between tests
done

kill $SERVER_PID
echo "Benchmark complete!"
```

---

## Integration Examples

### Python Script Integration

```python
#!/usr/bin/env python3
"""
Automated stress testing with Python
"""
import requests
import time
import json
from datetime import datetime

class StressTestManager:
    def __init__(self, base_url="http://localhost:8080"):
        self.base_url = base_url
        
    def get_system_info(self):
        response = requests.get(f"{self.base_url}/api/system-info")
        return response.json()
    
    def start_test(self, test_type, duration):
        response = requests.get(
            f"{self.base_url}/api/start-test",
            params={"type": test_type, "duration": duration}
        )
        return response.json()
    
    def get_status(self):
        response = requests.get(f"{self.base_url}/api/test-status")
        return response.json()
    
    def stop_test(self):
        response = requests.get(f"{self.base_url}/api/stop-test")
        return response.json()
    
    def run_monitored_test(self, test_type, duration, log_file=None):
        print(f"Starting {test_type} test for {duration} seconds...")
        
        # Start test
        result = self.start_test(test_type, duration)
        if not result['success']:
            print(f"Failed to start test: {result['message']}")
            return False
        
        # Monitor test
        data_points = []
        start_time = time.time()
        
        try:
            while True:
                status = self.get_status()
                
                if not status['running']:
                    break
                
                # Record data point
                data_point = {
                    'timestamp': datetime.now().isoformat(),
                    'elapsed': time.time() - start_time,
                    'cpu_usage': status['cpuUsage'],
                    'ram_usage': status['ramUsage'],
                    'cpu_temp': status['cpuTemp']
                }
                data_points.append(data_point)
                
                # Print progress
                print(f"[{data_point['elapsed']:.0f}s] "
                      f"CPU: {status['cpuUsage']:.1f}% | "
                      f"RAM: {status['ramUsage']:.1f}% | "
                      f"Temp: {status['cpuTemp']:.1f}°C")
                
                time.sleep(1)
                
        except KeyboardInterrupt:
            print("\nStopping test...")
            self.stop_test()
        
        # Save data if log file specified
        if log_file:
            with open(log_file, 'w') as f:
                json.dump({
                    'test_type': test_type,
                    'duration': duration,
                    'data_points': data_points
                }, f, indent=2)
            print(f"Data saved to {log_file}")
        
        return True

# Usage
if __name__ == "__main__":
    manager = StressTestManager()
    
    # Get system info
    info = manager.get_system_info()
    print("System Information:")
    print(f"CPU: {info['cpuModel']}")
    print(f"Cores: {info['cpuCores']}")
    print(f"RAM: {info['totalRAM']} MB")
    print()
    
    # Run test
    manager.run_monitored_test(
        test_type="cpu_multi",
        duration=60,
        log_file="test_results.json"
    )
```

### Node.js Integration

```javascript
// stress-test-client.js
const axios = require('axios');

class StressTestClient {
    constructor(baseURL = 'http://localhost:8080') {
        this.baseURL = baseURL;
        this.api = axios.create({ baseURL });
    }
    
    async getSystemInfo() {
        const response = await this.api.get('/api/system-info');
        return response.data;
    }
    
    async startTest(type, duration) {
        const response = await this.api.get('/api/start-test', {
            params: { type, duration }
        });
        return response.data;
    }
    
    async getStatus() {
        const response = await this.api.get('/api/test-status');
        return response.data;
    }
    
    async stopTest() {
        const response = await this.api.get('/api/stop-test');
        return response.data;
    }
    
    async monitorTest(intervalMs = 1000) {
        return new Promise((resolve) => {
            const interval = setInterval(async () => {
                const status = await this.getStatus();
                
                if (!status.running) {
                    clearInterval(interval);
                    resolve(status);
                    return;
                }
                
                console.log(`CPU: ${status.cpuUsage.toFixed(1)}% | ` +
                           `RAM: ${status.ramUsage.toFixed(1)}% | ` +
                           `Temp: ${status.cpuTemp.toFixed(1)}°C`);
            }, intervalMs);
        });
    }
}

// Usage
(async () => {
    const client = new StressTestClient();
    
    // Get system info
    const info = await client.getSystemInfo();
    console.log('System:', info.cpuModel);
    
    // Start test
    await client.startTest('cpu_multi', 60);
    console.log('Test started...');
    
    // Monitor test
    await client.monitorTest();
    console.log('Test completed!');
})();
```

---

## Troubleshooting Examples

### Example 1: Port Already in Use

```bash
# Problem: Can't start because port 8080 is busy

# Solution 1: Find what's using the port
lsof -i :8080
# or
netstat -tulpn | grep 8080

# Solution 2: Use different port
./stress_test 9000

# Solution 3: Kill process using the port
kill $(lsof -t -i:8080)
```

### Example 2: Temperature Not Showing

```bash
# Problem: CPU temperature shows 0°C

# Check thermal zones
ls /sys/class/thermal/

# Try reading manually
cat /sys/class/thermal/thermal_zone0/temp

# If permission denied
sudo chmod +r /sys/class/thermal/thermal_zone0/temp

# Or run with sudo (not recommended for production)
sudo ./stress_test
```

### Example 3: Build Errors

```bash
# Problem: Compilation fails

# Check compiler version
g++ --version

# Install/update compiler
sudo apt-get install build-essential

# Clean and rebuild
make clean
make

# Verbose build to see errors
make VERBOSE=1
```

### Example 4: Test Won't Stop

```bash
# Problem: Test continues even after clicking stop

# Force stop via API
curl http://localhost:8080/api/stop-test

# If that doesn't work, kill the process
pkill stress_test

# Or find and kill
ps aux | grep stress_test
kill <PID>
```

---

**MIT License © 2026 kj-devvixon**
