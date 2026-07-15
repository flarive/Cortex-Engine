#include "../../include/tools/system_monitor.h"




#include <iostream>
#include <cstring>

std::string engine::SystemMonitor::GetGPUVendor() {
    return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
}

std::string engine::SystemMonitor::GetGPURenderer() {
    return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

std::string engine::SystemMonitor::GetGPUVersion() {
    return reinterpret_cast<const char*>(glGetString(GL_VERSION));
}


uint64_t engine::SystemMonitor::getProcessRAM()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
    {
        return pmc.WorkingSetSize; // bytes
    }
    return 0;
}

double engine::SystemMonitor::getCPUTotalUsed()
{
    static uint64_t prevIdle = 0;
    static uint64_t prevTotal = 0;

    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime))
        return 0.0;

    uint64_t idle = fileTimeToUint64(idleTime);
    uint64_t kernel = fileTimeToUint64(kernelTime);
    uint64_t user = fileTimeToUint64(userTime);

    uint64_t total = idle + kernel + user;

    uint64_t idleDelta = idle - prevIdle;
    uint64_t totalDelta = total - prevTotal;

    prevIdle = idle;
    prevTotal = total;

    if (totalDelta == 0)
        return 0.0;

    double cpu = (1.0 - double(idleDelta) / double(totalDelta)) * 100.0;

    SYSTEM_INFO info;
    GetSystemInfo(&info);
    cpu /= info.dwNumberOfProcessors;   // REQUIRED

    return cpu;
}


//double engine::SystemMonitor::getCPUTotalUsed()
//{
//    double cpu = 0.0;
//    
//    FILETIME idleTime, kernelTime, userTime;
//    if (GetSystemTimes(&idleTime, &kernelTime, &userTime))
//    {
//        uint64_t idle = fileTimeToUint64(idleTime);
//        uint64_t kernel = fileTimeToUint64(kernelTime);
//        uint64_t user = fileTimeToUint64(userTime);
//
//        uint64_t total = idle + kernel + user;
//
//        uint64_t idleDelta = idle - prevIdle;
//        uint64_t totalDelta = total - prevTotal;
//
//        prevIdle = idle;
//        prevTotal = total;
//
//        if (totalDelta > 0)
//        {
//            double idleFrac = static_cast<double>(idleDelta) / static_cast<double>(totalDelta);
//
//            // Raw total CPU load across all cores
//            // same as "\Processor(_Total)\% Processor Time"
//            cpu = (1.0 - idleFrac) * 100.0;
//
//            // Normalize to match Task Manager
//            SYSTEM_INFO sysInfo;
//            GetSystemInfo(&sysInfo);
//            cpu /= (sysInfo.dwNumberOfProcessors);
//
//            if (cpu < 0.0) cpu = 0.0;
//            if (cpu > 100.0) cpu = 100.0;
//        }
//    }
//
//    return cpu;
//}

double engine::SystemMonitor::getCPUTotalUsedPDH()
{
    /*if (m_cpuPDHCounterExists)
    {*/
        return m_PDHCounters.getCPUTotalUsedPDH();
    //}
    //
    //return 0.0;
}


double engine::SystemMonitor::getProcessCPU()
{
    static uint64_t prevProc = 0;
    static uint64_t prevSys = 0;
    static uint64_t prevTime = GetTickCount64();
    static double   lastCPU = 0.0;

    uint64_t now = GetTickCount64();
    uint64_t elapsed = now - prevTime;

    // Only compute every 1000 ms (Task Manager interval)
    if (elapsed < 1000)
        return lastCPU;

    FILETIME ftSysIdle, ftSysKernel, ftSysUser;
    FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

    GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser);
    GetProcessTimes(GetCurrentProcess(),
        &ftProcCreation, &ftProcExit,
        &ftProcKernel, &ftProcUser);

    auto to64 = [](const FILETIME& ft)
        {
            return (uint64_t(ft.dwHighDateTime) << 32) |
                uint64_t(ft.dwLowDateTime);
        };

    uint64_t sysTotal = to64(ftSysKernel) + to64(ftSysUser);
    uint64_t procTotal = to64(ftProcKernel) + to64(ftProcUser);

    uint64_t sysDelta = sysTotal - prevSys;
    uint64_t procDelta = procTotal - prevProc;

    prevSys = sysTotal;
    prevProc = procTotal;
    prevTime = now;

    if (sysDelta == 0)
        return lastCPU;

    double cpu = double(procDelta) / double(sysDelta);

    SYSTEM_INFO info;
    GetSystemInfo(&info);
    cpu /= info.dwNumberOfProcessors;

    cpu *= 100.0;

    if (cpu < 0.0) cpu = 0.0;
    if (cpu > 100.0) cpu = 100.0;

    lastCPU = cpu;
    return cpu;
}

