#include "stress_test.h"
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

// Simple embedded HTTP server (single-header style)
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <signal.h>

class SimpleHTTPServer {
private:
    int serverSocket;
    int port;
    std::atomic<bool> running;
    std::thread serverThread;
    std::shared_ptr<StressTest::StressTestBase> currentTest;
    std::mutex testMutex;
    
public:
    SimpleHTTPServer(int p = 8080) : port(p), running(false), serverSocket(-1) {}
    
    ~SimpleHTTPServer() {
        stop();
    }
    
    void start() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            std::cerr << "Failed to create socket" << std::endl;
            return;
        }
        
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        
        if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Failed to bind socket" << std::endl;
            close(serverSocket);
            return;
        }
        
        if (listen(serverSocket, 3) < 0) {
            std::cerr << "Failed to listen" << std::endl;
            close(serverSocket);
            return;
        }
        
        running = true;
        serverThread = std::thread(&SimpleHTTPServer::acceptConnections, this);
        
        std::cout << "Server started on http://localhost:" << port << std::endl;
    }
    
    void stop() {
        running = false;
        if (serverSocket >= 0) {
            close(serverSocket);
            serverSocket = -1;
        }
        if (serverThread.joinable()) {
            serverThread.join();
        }
    }
    
private:
    void acceptConnections() {
        while (running) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
            if (clientSocket < 0) {
                if (running) {
                    std::cerr << "Accept failed" << std::endl;
                }
                continue;
            }
            
            std::thread(&SimpleHTTPServer::handleClient, this, clientSocket).detach();
        }
    }
    
    void handleClient(int clientSocket) {
        char buffer[4096] = {0};
        read(clientSocket, buffer, sizeof(buffer) - 1);
        
        std::string request(buffer);
        std::string response;
        
        // Parse request
        size_t methodEnd = request.find(' ');
        size_t pathEnd = request.find(' ', methodEnd + 1);
        std::string method = request.substr(0, methodEnd);
        std::string path = request.substr(methodEnd + 1, pathEnd - methodEnd - 1);
        
        // Route handling
        if (path == "/" || path.find("/index.html") != std::string::npos) {
            response = getHTMLPage();
            sendResponse(clientSocket, "200 OK", "text/html", response);
        }
        else if (path == "/api/system-info") {
            response = getSystemInfoJSON();
            sendResponse(clientSocket, "200 OK", "application/json", response);
        }
        else if (path.find("/api/start-test") != std::string::npos) {
            size_t typePos = path.find("type=");
            size_t durPos = path.find("duration=");
            if (typePos != std::string::npos && durPos != std::string::npos) {
                std::string testType = path.substr(typePos + 5, path.find('&', typePos) - typePos - 5);
                std::string durationStr = path.substr(durPos + 9);
                int duration = std::stoi(durationStr);
                response = startTest(testType, duration);
                sendResponse(clientSocket, "200 OK", "application/json", response);
            } else {
                sendResponse(clientSocket, "400 Bad Request", "application/json", "{\"error\":\"Invalid parameters\"}");
            }
        }
        else if (path == "/api/stop-test") {
            response = stopTest();
            sendResponse(clientSocket, "200 OK", "application/json", response);
        }
        else if (path == "/api/test-status") {
            response = getTestStatus();
            sendResponse(clientSocket, "200 OK", "application/json", response);
        }
        else {
            sendResponse(clientSocket, "404 Not Found", "text/plain", "Not Found");
        }
        
        close(clientSocket);
    }
    
    void sendResponse(int socket, const std::string& status, 
                     const std::string& contentType, const std::string& body) {
        std::ostringstream response;
        response << "HTTP/1.1 " << status << "\r\n";
        response << "Content-Type: " << contentType << "\r\n";
        response << "Content-Length: " << body.length() << "\r\n";
        response << "Access-Control-Allow-Origin: *\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << body;
        
        std::string responseStr = response.str();
        write(socket, responseStr.c_str(), responseStr.length());
    }
    
    std::string getHTMLPage();
    std::string getSystemInfoJSON();
    std::string startTest(const std::string& type, int duration);
    std::string stopTest();
    std::string getTestStatus();
};

std::string SimpleHTTPServer::getSystemInfoJSON() {
    StressTest::SystemInfo info = StressTest::SystemInfoGatherer::getSystemInfo();
    
    std::ostringstream json;
    json << "{"
         << "\"cpuModel\":\"" << info.cpuModel << "\","
         << "\"cpuCores\":" << info.cpuCores << ","
         << "\"cpuThreads\":" << info.cpuThreads << ","
         << "\"cpuFrequency\":" << info.cpuFrequency << ","
         << "\"totalRAM\":" << info.totalRAM << ","
         << "\"availableRAM\":" << info.availableRAM << ","
         << "\"gpuInfo\":\"" << info.gpuInfo << "\""
         << "}";
    
    return json.str();
}

std::string SimpleHTTPServer::startTest(const std::string& type, int duration) {
    std::lock_guard<std::mutex> lock(testMutex);
    
    if (currentTest && currentTest->isRunning()) {
        return "{\"success\":false,\"message\":\"Test already running\"}";
    }
    
    if (type == "cpu_single") {
        currentTest = std::make_shared<StressTest::CPUSingleCoreTest>(duration);
    } else if (type == "cpu_multi") {
        currentTest = std::make_shared<StressTest::CPUMultiCoreTest>(duration);
    } else if (type == "ram_test") {
        currentTest = std::make_shared<StressTest::RAMStressTest>(duration);
    } else if (type == "gpu_test") {
        currentTest = std::make_shared<StressTest::GPUStressTest>(duration);
    } else if (type == "combined") {
        currentTest = std::make_shared<StressTest::CombinedStressTest>(duration);
    } else {
        return "{\"success\":false,\"message\":\"Unknown test type\"}";
    }
    
    currentTest->start();
    return "{\"success\":true,\"message\":\"Test started\"}";
}

std::string SimpleHTTPServer::stopTest() {
    std::lock_guard<std::mutex> lock(testMutex);
    
    if (currentTest) {
        currentTest->stop();
        return "{\"success\":true,\"message\":\"Test stopped\"}";
    }
    
    return "{\"success\":false,\"message\":\"No test running\"}";
}

std::string SimpleHTTPServer::getTestStatus() {
    std::lock_guard<std::mutex> lock(testMutex);
    
    if (!currentTest) {
        return "{\"running\":false,\"completed\":false}";
    }
    
    bool running = currentTest->isRunning();
    bool completed = currentTest->isCompleted();
    auto stats = currentTest->getStats();
    
    std::ostringstream json;
    json << "{"
         << "\"running\":" << (running ? "true" : "false") << ","
         << "\"completed\":" << (completed ? "true" : "false") << ",";
    
    if (!stats.empty()) {
        auto& lastStat = stats.back();
        json << "\"cpuUsage\":" << lastStat.cpuUsage << ","
             << "\"ramUsage\":" << lastStat.ramUsage << ","
             << "\"ramUsedMB\":" << lastStat.ramUsedMB << ","
             << "\"cpuTemp\":" << lastStat.cpuTemp;
    } else {
        json << "\"cpuUsage\":0,"
             << "\"ramUsage\":0,"
             << "\"ramUsedMB\":0,"
             << "\"cpuTemp\":0";
    }
    
    json << "}";
    return json.str();
}

std::string SimpleHTTPServer::getHTMLPage() {
    return R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hardware Stress Test Suite</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }
        
        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            text-align: center;
        }
        
        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
        }
        
        .header p {
            font-size: 1.1em;
            opacity: 0.9;
        }
        
        .content {
            padding: 30px;
        }
        
        .system-info {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 30px;
        }
        
        .system-info h2 {
            color: #667eea;
            margin-bottom: 15px;
            font-size: 1.5em;
        }
        
        .info-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 15px;
        }
        
        .info-item {
            background: white;
            padding: 15px;
            border-radius: 8px;
            border-left: 4px solid #667eea;
        }
        
        .info-item label {
            display: block;
            font-weight: bold;
            color: #555;
            margin-bottom: 5px;
        }
        
        .info-item span {
            color: #333;
            font-size: 1.1em;
        }
        
        .test-section {
            margin-bottom: 30px;
        }
        
        .test-section h2 {
            color: #667eea;
            margin-bottom: 20px;
            font-size: 1.5em;
        }
        
        .test-options {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-bottom: 20px;
        }
        
        .test-option {
            background: #f8f9fa;
            padding: 20px;
            border-radius: 10px;
            cursor: pointer;
            transition: all 0.3s;
            border: 3px solid transparent;
        }
        
        .test-option:hover {
            transform: translateY(-5px);
            box-shadow: 0 5px 15px rgba(0,0,0,0.1);
        }
        
        .test-option.selected {
            border-color: #667eea;
            background: #e8eaf6;
        }
        
        .test-option h3 {
            color: #667eea;
            margin-bottom: 10px;
        }
        
        .test-option p {
            color: #666;
            font-size: 0.9em;
        }
        
        .controls {
            display: flex;
            gap: 15px;
            align-items: center;
            flex-wrap: wrap;
            margin-bottom: 30px;
        }
        
        .controls label {
            font-weight: bold;
            color: #555;
        }
        
        .controls input {
            padding: 10px;
            border: 2px solid #ddd;
            border-radius: 8px;
            font-size: 1em;
            width: 150px;
        }
        
        .controls button {
            padding: 12px 30px;
            border: none;
            border-radius: 8px;
            font-size: 1em;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s;
        }
        
        .btn-start {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }
        
        .btn-start:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
        }
        
        .btn-start:disabled {
            background: #ccc;
            cursor: not-allowed;
            transform: none;
        }
        
        .btn-stop {
            background: #e74c3c;
            color: white;
        }
        
        .btn-stop:hover {
            background: #c0392b;
        }
        
        .btn-stop:disabled {
            background: #ccc;
            cursor: not-allowed;
        }
        
        .status {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 30px;
        }
        
        .status h2 {
            color: #667eea;
            margin-bottom: 15px;
        }
        
        .status-badge {
            display: inline-block;
            padding: 8px 20px;
            border-radius: 20px;
            font-weight: bold;
            margin-bottom: 15px;
        }
        
        .status-badge.ready {
            background: #d4edda;
            color: #155724;
        }
        
        .status-badge.running {
            background: #fff3cd;
            color: #856404;
        }
        
        .status-badge.completed {
            background: #d1ecf1;
            color: #0c5460;
        }
        
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-top: 15px;
        }
        
        .stat-card {
            background: white;
            padding: 20px;
            border-radius: 8px;
            text-align: center;
        }
        
        .stat-card h3 {
            color: #555;
            font-size: 0.9em;
            margin-bottom: 10px;
        }
        
        .stat-card .value {
            font-size: 2em;
            font-weight: bold;
            color: #667eea;
        }
        
        .stat-card .unit {
            font-size: 0.8em;
            color: #999;
        }
        
        .progress-bar {
            width: 100%;
            height: 30px;
            background: #e0e0e0;
            border-radius: 15px;
            overflow: hidden;
            margin-top: 15px;
        }
        
        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #667eea 0%, #764ba2 100%);
            transition: width 0.3s;
            display: flex;
            align-items: center;
            justify-content: center;
            color: white;
            font-weight: bold;
        }
        
        .footer {
            text-align: center;
            padding: 20px;
            color: #666;
            background: #f8f9fa;
        }
        
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
        
        .running-indicator {
            animation: pulse 1.5s infinite;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>⚡ Hardware Stress Test Suite</h1>
            <p>Professional hardware testing tool by kj-devvixon</p>
        </div>
        
        <div class="content">
            <div class="system-info">
                <h2>🖥️ System Information</h2>
                <div class="info-grid" id="systemInfo">
                    <div class="info-item">
                        <label>CPU Model:</label>
                        <span id="cpuModel">Loading...</span>
                    </div>
                    <div class="info-item">
                        <label>CPU Cores/Threads:</label>
                        <span id="cpuCores">Loading...</span>
                    </div>
                    <div class="info-item">
                        <label>CPU Frequency:</label>
                        <span id="cpuFreq">Loading...</span>
                    </div>
                    <div class="info-item">
                        <label>Total RAM:</label>
                        <span id="totalRAM">Loading...</span>
                    </div>
                    <div class="info-item">
                        <label>Available RAM:</label>
                        <span id="availRAM">Loading...</span>
                    </div>
                    <div class="info-item">
                        <label>GPU:</label>
                        <span id="gpu">Loading...</span>
                    </div>
                </div>
            </div>
            
            <div class="test-section">
                <h2>🔥 Select Test Type</h2>
                <div class="test-options">
                    <div class="test-option selected" data-test="cpu_single">
                        <h3>CPU Single-Core</h3>
                        <p>Tests single thread performance</p>
                    </div>
                    <div class="test-option" data-test="cpu_multi">
                        <h3>CPU Multi-Core</h3>
                        <p>Tests all cores simultaneously</p>
                    </div>
                    <div class="test-option" data-test="ram_test">
                        <h3>RAM Stress</h3>
                        <p>Memory allocation and access test</p>
                    </div>
                    <div class="test-option" data-test="gpu_test">
                        <h3>GPU Stress</h3>
                        <p>Graphics processing test</p>
                    </div>
                    <div class="test-option" data-test="combined">
                        <h3>Combined Test</h3>
                        <p>All components simultaneously</p>
                    </div>
                </div>
                
                <div class="controls">
                    <label>Duration:</label>
                    <input type="number" id="duration" value="60" min="1" max="3600">
                    <span>seconds</span>
                    <button class="btn-start" id="startBtn">Start Test</button>
                    <button class="btn-stop" id="stopBtn" disabled>Stop Test</button>
                </div>
            </div>
            
            <div class="status">
                <h2>📊 Status</h2>
                <div class="status-badge ready" id="statusBadge">Ready</div>
                <div class="progress-bar">
                    <div class="progress-fill" id="progressBar" style="width: 0%"></div>
                </div>
                
                <div class="stats-grid">
                    <div class="stat-card">
                        <h3 id="statLabel1">CPU Usage</h3>
                        <div class="value" id="cpuUsage">0</div>
                        <div class="unit">%</div>
                    </div>
                    <div class="stat-card">
                        <h3 id="statLabel2">RAM Usage</h3>
                        <div class="value" id="ramUsage">0</div>
                        <div class="unit">%</div>
                    </div>
                    <div class="stat-card">
                        <h3 id="statLabel3">RAM Used</h3>
                        <div class="value" id="ramUsedMB">0</div>
                        <div class="unit">MB</div>
                    </div>
                    <div class="stat-card">
                        <h3 id="statLabel4">CPU Temp</h3>
                        <div class="value" id="cpuTemp">0</div>
                        <div class="unit">°C</div>
                    </div>
                </div>
            </div>
        </div>
        
        <div class="footer">
            <p>MIT License © 2026 kj-devvixon | Hardware Stress Test Suite v1.0</p>
        </div>
    </div>
    
    <script>
        let selectedTest = 'cpu_single';
        let testRunning = false;
        let statusInterval = null;
        
        // Load system info
        fetch('/api/system-info')
            .then(r => r.json())
            .then(data => {
                document.getElementById('cpuModel').textContent = data.cpuModel;
                document.getElementById('cpuCores').textContent = `${data.cpuCores} cores / ${data.cpuThreads} threads`;
                document.getElementById('cpuFreq').textContent = `${data.cpuFrequency.toFixed(0)} MHz`;
                document.getElementById('totalRAM').textContent = `${(data.totalRAM / 1024).toFixed(2)} GB`;
                document.getElementById('availRAM').textContent = `${(data.availableRAM / 1024).toFixed(2)} GB`;
                document.getElementById('gpu').textContent = data.gpuInfo.substring(0, 50);
            });
        
        // Test option selection
        document.querySelectorAll('.test-option').forEach(opt => {
            opt.addEventListener('click', () => {
                document.querySelectorAll('.test-option').forEach(o => o.classList.remove('selected'));
                opt.classList.add('selected');
                selectedTest = opt.dataset.test;
                updateStatLabels(selectedTest);
            });
        });
        
        // Function to update stat labels based on test type
        function updateStatLabels(testType) {
            const label1 = document.getElementById('statLabel1');
            const label2 = document.getElementById('statLabel2');
            const label3 = document.getElementById('statLabel3');
            const label4 = document.getElementById('statLabel4');
            
            switch(testType) {
                case 'gpu_test':
                    label1.textContent = 'GPU Usage';
                    label2.textContent = 'VRAM Usage';
                    label3.textContent = 'VRAM Used';
                    label4.textContent = 'GPU Temp';
                    break;
                case 'ram_test':
                    label1.textContent = 'CPU Usage';
                    label2.textContent = 'RAM Usage';
                    label3.textContent = 'RAM Used';
                    label4.textContent = 'RAM Temp';
                    break;
                case 'cpu_single':
                case 'cpu_multi':
                    label1.textContent = 'CPU Usage';
                    label2.textContent = 'RAM Usage';
                    label3.textContent = 'RAM Used';
                    label4.textContent = 'CPU Temp';
                    break;
                case 'combined':
                    label1.textContent = 'CPU Usage';
                    label2.textContent = 'RAM Usage';
                    label3.textContent = 'RAM Used';
                    label4.textContent = 'System Temp';
                    break;
                default:
                    label1.textContent = 'CPU Usage';
                    label2.textContent = 'RAM Usage';
                    label3.textContent = 'RAM Used';
                    label4.textContent = 'CPU Temp';
            }
        }
        
        // Start button
        document.getElementById('startBtn').addEventListener('click', () => {
            const duration = document.getElementById('duration').value;
            
            fetch(`/api/start-test?type=${selectedTest}&duration=${duration}`)
                .then(r => r.json())
                .then(data => {
                    if (data.success) {
                        testRunning = true;
                        document.getElementById('startBtn').disabled = true;
                        document.getElementById('stopBtn').disabled = false;
                        document.getElementById('statusBadge').textContent = 'Running';
                        document.getElementById('statusBadge').className = 'status-badge running running-indicator';
                        
                        statusInterval = setInterval(updateStatus, 500);
                    }
                });
        });
        
        // Stop button
        document.getElementById('stopBtn').addEventListener('click', () => {
            fetch('/api/stop-test')
                .then(r => r.json())
                .then(data => {
                    testRunning = false;
                    document.getElementById('startBtn').disabled = false;
                    document.getElementById('stopBtn').disabled = true;
                    document.getElementById('statusBadge').textContent = 'Stopped';
                    document.getElementById('statusBadge').className = 'status-badge completed';
                    
                    if (statusInterval) {
                        clearInterval(statusInterval);
                    }
                });
        });
        
        function updateStatus() {
            fetch('/api/test-status')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('cpuUsage').textContent = data.cpuUsage.toFixed(1);
                    document.getElementById('ramUsage').textContent = data.ramUsage.toFixed(1);
                    document.getElementById('ramUsedMB').textContent = data.ramUsedMB;
                    document.getElementById('cpuTemp').textContent = data.cpuTemp.toFixed(1);
                    
                    if (!data.running && testRunning) {
                        testRunning = false;
                        document.getElementById('startBtn').disabled = false;
                        document.getElementById('stopBtn').disabled = true;
                        document.getElementById('statusBadge').textContent = 'Completed';
                        document.getElementById('statusBadge').className = 'status-badge completed';
                        
                        if (statusInterval) {
                            clearInterval(statusInterval);
                        }
                    }
                });
        }
    </script>
</body>
</html>)";
}

// Global server instance
SimpleHTTPServer* globalServer = nullptr;

void signalHandler(int signum) {
    std::cout << "\nShutting down server..." << std::endl;
    if (globalServer) {
        globalServer->stop();
    }
    exit(signum);
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    std::cout << "========================================" << std::endl;
    std::cout << " Hardware Stress Test Suite v1.0" << std::endl;
    std::cout << " MIT License © 2026 kj-devvixon" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    int port = 8080;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }
    
    SimpleHTTPServer server(port);
    globalServer = &server;
    
    server.start();
    
    std::cout << "\nOpen your browser and navigate to:" << std::endl;
    std::cout << "http://localhost:" << port << std::endl;
    std::cout << "\nPress Ctrl+C to stop the server." << std::endl;
    
    // Keep the main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
