# API Documentation - Hardware Stress Test Suite

This document describes the REST API endpoints provided by the Hardware Stress Test Suite.

**Base URL:** `http://localhost:8080` (default)

## Table of Contents
- [System Information](#system-information)
- [Test Management](#test-management)
- [Test Status](#test-status)
- [Response Formats](#response-formats)

---

## System Information

### Get System Information

Retrieves detailed information about the host system.

**Endpoint:** `GET /api/system-info`

**Response:** JSON

**Response Fields:**
- `cpuModel` (string) - CPU model name
- `cpuCores` (integer) - Number of physical CPU cores
- `cpuThreads` (integer) - Number of logical CPU threads
- `cpuFrequency` (float) - CPU frequency in MHz
- `totalRAM` (integer) - Total RAM in MB
- `availableRAM` (integer) - Available RAM in MB
- `gpuInfo` (string) - GPU information

**Example Request:**
```bash
curl http://localhost:8080/api/system-info
```

**Example Response:**
```json
{
  "cpuModel": "Intel(R) Core(TM) i7-9700K CPU @ 3.60GHz",
  "cpuCores": 8,
  "cpuThreads": 8,
  "cpuFrequency": 3600.0,
  "totalRAM": 16384,
  "availableRAM": 8192,
  "gpuInfo": "NVIDIA GeForce RTX 2080"
}
```

---

## Test Management

### Start Test

Starts a stress test of the specified type.

**Endpoint:** `GET /api/start-test?type={test_type}&duration={seconds}`

**Query Parameters:**
- `type` (string, required) - Type of test to run:
  - `cpu_single` - Single-core CPU test
  - `cpu_multi` - Multi-core CPU test
  - `ram_test` - RAM stress test
  - `gpu_test` - GPU stress test
  - `combined` - All tests combined
- `duration` (integer, required) - Test duration in seconds (1-3600)

**Response:** JSON

**Response Fields:**
- `success` (boolean) - Whether test started successfully
- `message` (string) - Status message

**Example Request:**
```bash
curl "http://localhost:8080/api/start-test?type=cpu_multi&duration=60"
```

**Example Success Response:**
```json
{
  "success": true,
  "message": "Test started"
}
```

**Example Error Response:**
```json
{
  "success": false,
  "message": "Test already running"
}
```

**Possible Error Messages:**
- "Test already running" - A test is currently active
- "Unknown test type" - Invalid test type specified
- "Invalid duration" - Duration out of range

---

### Stop Test

Stops the currently running test.

**Endpoint:** `GET /api/stop-test`

**Response:** JSON

**Response Fields:**
- `success` (boolean) - Whether test stopped successfully
- `message` (string) - Status message

**Example Request:**
```bash
curl http://localhost:8080/api/stop-test
```

**Example Success Response:**
```json
{
  "success": true,
  "message": "Test stopped"
}
```

**Example Error Response:**
```json
{
  "success": false,
  "message": "No test running"
}
```

---

## Test Status

### Get Test Status

Retrieves the current status and real-time statistics of the running test.

**Endpoint:** `GET /api/test-status`

**Response:** JSON

**Response Fields:**
- `running` (boolean) - Whether a test is currently running
- `completed` (boolean) - Whether the test has completed
- `cpuUsage` (float) - Current CPU usage percentage (0-100)
- `ramUsage` (float) - Current RAM usage percentage (0-100)
- `ramUsedMB` (integer) - Current RAM used in megabytes
- `cpuTemp` (float) - Current CPU temperature in Celsius (0 if unavailable)

**Example Request:**
```bash
curl http://localhost:8080/api/test-status
```

**Example Response (Test Running):**
```json
{
  "running": true,
  "completed": false,
  "cpuUsage": 98.5,
  "ramUsage": 45.2,
  "ramUsedMB": 7400,
  "cpuTemp": 72.0
}
```

**Example Response (No Test):**
```json
{
  "running": false,
  "completed": false,
  "cpuUsage": 0,
  "ramUsage": 0,
  "ramUsedMB": 0,
  "cpuTemp": 0
}
```

**Example Response (Test Completed):**
```json
{
  "running": false,
  "completed": true,
  "cpuUsage": 15.3,
  "ramUsage": 28.1,
  "ramUsedMB": 4600,
  "cpuTemp": 45.0
}
```

---

## Response Formats

### Success Response
All successful API calls return HTTP 200 OK with JSON body.

### Error Response
Error responses return appropriate HTTP status codes:
- `400 Bad Request` - Invalid parameters
- `404 Not Found` - Endpoint not found
- `500 Internal Server Error` - Server error

---

## Usage Examples

### JavaScript (Browser)

```javascript
// Get system info
fetch('/api/system-info')
  .then(response => response.json())
  .then(data => console.log(data));

// Start test
fetch('/api/start-test?type=cpu_multi&duration=60')
  .then(response => response.json())
  .then(data => console.log(data));

// Get status (polling)
setInterval(() => {
  fetch('/api/test-status')
    .then(response => response.json())
    .then(data => {
      console.log(`CPU: ${data.cpuUsage}%`);
      console.log(`RAM: ${data.ramUsage}%`);
      console.log(`Temp: ${data.cpuTemp}°C`);
    });
}, 500);

// Stop test
fetch('/api/stop-test')
  .then(response => response.json())
  .then(data => console.log(data));
```

### Python

```python
import requests
import time

base_url = "http://localhost:8080"

# Get system info
response = requests.get(f"{base_url}/api/system-info")
system_info = response.json()
print(f"CPU: {system_info['cpuModel']}")
print(f"RAM: {system_info['totalRAM']} MB")

# Start test
response = requests.get(
    f"{base_url}/api/start-test",
    params={"type": "cpu_multi", "duration": 60}
)
result = response.json()
print(result['message'])

# Monitor test
if result['success']:
    while True:
        response = requests.get(f"{base_url}/api/test-status")
        status = response.json()
        
        if not status['running']:
            break
            
        print(f"CPU: {status['cpuUsage']:.1f}% | "
              f"RAM: {status['ramUsage']:.1f}% | "
              f"Temp: {status['cpuTemp']:.1f}°C")
        
        time.sleep(1)

print("Test completed!")
```

### Bash

```bash
#!/bin/bash

BASE_URL="http://localhost:8080"

# Get system info
echo "System Information:"
curl -s "$BASE_URL/api/system-info" | jq

# Start test
echo -e "\nStarting CPU multi-core test for 60 seconds..."
curl -s "$BASE_URL/api/start-test?type=cpu_multi&duration=60" | jq

# Monitor test
echo -e "\nMonitoring test..."
while true; do
    status=$(curl -s "$BASE_URL/api/test-status")
    running=$(echo $status | jq -r '.running')
    
    if [ "$running" = "false" ]; then
        break
    fi
    
    cpu=$(echo $status | jq -r '.cpuUsage')
    ram=$(echo $status | jq -r '.ramUsage')
    temp=$(echo $status | jq -r '.cpuTemp')
    
    echo "CPU: ${cpu}% | RAM: ${ram}% | Temp: ${temp}°C"
    sleep 1
done

echo "Test completed!"
```

---

## Rate Limiting

Currently, there are no rate limits on the API. However, be mindful of:
- Only one test can run at a time
- Status endpoint should be polled at reasonable intervals (500ms - 1s recommended)
- Starting multiple tests simultaneously will return an error

---

## CORS

The API includes CORS headers allowing access from any origin:
```
Access-Control-Allow-Origin: *
```

This enables the API to be used from web applications hosted on different domains.

---

## Future API Endpoints (Planned)

The following endpoints are planned for future releases:

- `GET /api/test-history` - Get historical test results
- `GET /api/test-results/{id}` - Get specific test results
- `POST /api/export-results` - Export results to CSV/JSON
- `GET /api/config` - Get current configuration
- `POST /api/config` - Update configuration
- `WebSocket /ws/live-stats` - Real-time statistics stream

---

**MIT License © 2026 kj-devvixon**
