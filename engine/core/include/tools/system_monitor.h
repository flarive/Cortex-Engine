#pragma once

#include "../common_defines.h"
#include <string>
#include <deque>

class SystemMonitor final
{
public:
    SystemMonitor();
    ~SystemMonitor() = default;

    //double GetCPUUsage();
    //double GetMemoryUsage();
    std::string GetGPUVendor();
    std::string GetGPURenderer();
    std::string GetGPUVersion();

private:
//#ifdef _WIN32
//    void InitWindowsCPUUsage();
//#elif __linux__
//    long lastTotalUser, lastTotalNice, lastTotalSystem, lastTotalIdle;
//#endif
};
