#include "../../include/tools/SystemMonitor.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <thread> // For sleep
#pragma comment(lib, "Pdh.lib") // Link PDH Library

static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuTotal;

double lastCPUValue;

SystemMonitor::SystemMonitor() {
    InitWindowsCPUUsage();
}

void SystemMonitor::InitWindowsCPUUsage() {
    PDH_STATUS status;

    status = PdhOpenQuery(NULL, NULL, &cpuQuery);
    if (status != ERROR_SUCCESS) {
        std::cerr << "PdhOpenQuery failed with error: 0x" << std::hex << status << std::endl;
        return;
    }

    status = PdhAddEnglishCounter(cpuQuery, TEXT("\\Processor(0)\\% Processor Time"), NULL, &cpuTotal);
    if (status != ERROR_SUCCESS) {
        std::cerr << "PdhAddCounter failed: " << status << std::endl;
        return;
    }

    status = PdhCollectQueryData(cpuQuery); // First useless sample
    if (status != ERROR_SUCCESS) {
        std::cerr << "PdhCollectQueryData (init) failed: " << status << std::endl;
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Wait for second valid sample
}

SystemMonitor::~SystemMonitor() {
    PdhCloseQuery(cpuQuery);
}

double SystemMonitor::GetCPUUsage() {
    static auto lastCheck = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();

    // Wait at least 500ms between updates
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCheck).count() < 500) {
        return lastCPUValue; // Return last measured value
    }

    lastCheck = now;
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);

    lastCPUValue = counterVal.doubleValue;
    return lastCPUValue;
}

double SystemMonitor::GetMemoryUsage() {
    /*MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return (double)(memInfo.ullTotalPhys - memInfo.ullAvailPhys) / memInfo.ullTotalPhys * 100.0;*/

    PROCESS_MEMORY_COUNTERS_EX pmc;
    HANDLE hProcess = GetCurrentProcess(); // Get handle to this process

    if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        SIZE_T memoryUsed = pmc.PrivateUsage; // Memory used in bytes
        return static_cast<double>(memoryUsed) / (1024.0 * 1024.0); // Convert to MB
    }

    return -1.0; // Return -1 on error
}

#elif __linux__
#include <sys/sysinfo.h>
#include <fstream>

SystemMonitor::SystemMonitor() {
    lastTotalUser = lastTotalNice = lastTotalSystem = lastTotalIdle = 0;
}

SystemMonitor::~SystemMonitor() {}

double SystemMonitor::GetMemoryUsage() {
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    return (double)(memInfo.totalram - memInfo.freeram) / memInfo.totalram * 100.0;
}

double SystemMonitor::GetCPUUsage() {
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    file.close();

    long user, nice, system, idle;
    sscanf(line.c_str(), "cpu %ld %ld %ld %ld", &user, &nice, &system, &idle);

    long total = user + nice + system + idle;
    long totalDiff = total - (lastTotalUser + lastTotalNice + lastTotalSystem + lastTotalIdle);
    long idleDiff = idle - lastTotalIdle;

    lastTotalUser = user;
    lastTotalNice = nice;
    lastTotalSystem = system;
    lastTotalIdle = idle;

    return (double)(totalDiff - idleDiff) / totalDiff * 100.0;
}
#endif

std::string SystemMonitor::GetGPUVendor() {
    return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
}

std::string SystemMonitor::GetGPURenderer() {
    return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

std::string SystemMonitor::GetGPUVersion() {
    return reinterpret_cast<const char*>(glGetString(GL_VERSION));
}
