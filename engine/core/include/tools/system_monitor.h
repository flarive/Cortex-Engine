#pragma once

#include "../common_defines.h"
#include <cstdint>
#include <chrono>

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#else
#include <cstdio>
#include <unistd.h>
#endif




namespace engine
{
    #if defined(_WIN32)
    class PDHCounters;
    #endif
    
    class SystemMonitor final
    {
    public:
        SystemMonitor()
        {
#if defined(_WIN32)
            // CPU query
            initWindowsCPU();
#else
            readProcStat(prevIdle, prevTotal);
#endif
        }

        ~SystemMonitor() = default;
        
        

        // Call every ~200ms
        void update()
        {
#if defined(_WIN32)
            updateWindows();
#else
            updateLinux();
#endif
        }

        double getCPU() const { return cpuTotalUsedPercent; }
        double getCPUProcess() const { return cpuProcessPercent; }


        uint64_t getRAMUsed() const { return ramUsedBytes; }
        uint64_t getRAMTotal() const { return ramTotalBytes; }
        uint64_t getProcessRAM();
        

        std::string GetGPUVendor();
        std::string GetGPURenderer();
        std::string GetGPUVersion();




    private:
        
        #if defined(_WIN32)
        PDHCounters* m_PDHCounters{};
        #endif

        
        double cpuTotalUsedPercent = 0.0;
        double cpuProcessPercent = 0.0;
        uint64_t ramUsedBytes = 0;
        uint64_t ramTotalBytes = 0;




        double getCPUTotalUsed();
        double getCPUTotalUsedPDH();
        double getProcessCPU();


#if defined(_WIN32)

        uint64_t prevIdle = 0;
        uint64_t prevTotal = 0;

        static uint64_t fileTimeToUint64(const FILETIME& ft)
        {
            return (static_cast<uint64_t>(ft.dwHighDateTime) << 32) |
                static_cast<uint64_t>(ft.dwLowDateTime);
        }


        void initWindowsCPU()
        {
            FILETIME idleTime, kernelTime, userTime;
            if (GetSystemTimes(&idleTime, &kernelTime, &userTime))
            {
                uint64_t idle = fileTimeToUint64(idleTime);
                uint64_t kernel = fileTimeToUint64(kernelTime);
                uint64_t user = fileTimeToUint64(userTime);

                prevIdle = idle;
                prevTotal = idle + kernel + user;
            }
        }

        void updateWindows()
        {
            #if defined(_WIN32)
            cpuTotalUsedPercent = getCPUTotalUsedPDH(); // using Windows PDH (closest to Task manager value)
            #else
            cpuTotalUsedPercent = getCPUTotalUsed();
            #endif

            cpuProcessPercent = getProcessCPU();

            // RAM
            MEMORYSTATUSEX mem{};
            mem.dwLength = sizeof(mem);
            GlobalMemoryStatusEx(&mem);

            ramTotalBytes = mem.ullTotalPhys;
            ramUsedBytes = mem.ullTotalPhys - mem.ullAvailPhys;
        }
#else
        uint64_t prevIdle = 0, prevTotal = 0;

        static void readProcStat(uint64_t& idle, uint64_t& total)
        {
            FILE* file = fopen("/proc/stat", "r");
            if (!file) return;

            char cpu[5];
            uint64_t user, nice, system, idle_t, iowait, irq, softirq, steal;

            fscanf(file, "%s %lu %lu %lu %lu %lu %lu %lu %lu",
                cpu, &user, &nice, &system, &idle_t, &iowait, &irq, &softirq, &steal);
            fclose(file);

            idle = idle_t + iowait;
            total = user + nice + system + idle_t + iowait + irq + softirq + steal;
        }

        void updateLinux()
        {
            // CPU
            uint64_t idle, total;
            readProcStat(idle, total);

            uint64_t idleDelta = idle - prevIdle;
            uint64_t totalDelta = total - prevTotal;

            prevIdle = idle;
            prevTotal = total;

            if (totalDelta > 0)
                cpuPercent = (1.0 - (double(idleDelta) / double(totalDelta))) * 100.0;

            // RAM
            FILE* file = fopen("/proc/meminfo", "r");
            if (!file) return;

            char label[64];
            uint64_t value;
            char unit[8];

            uint64_t memTotal = 0;
            uint64_t memAvailable = 0;

            while (fscanf(file, "%63s %lu %7s", label, &value, unit) != EOF)
            {
                if (strcmp(label, "MemTotal:") == 0)
                    memTotal = value * 1024;
                else if (strcmp(label, "MemAvailable:") == 0)
                    memAvailable = value * 1024;
            }
            fclose(file);

            ramTotalBytes = memTotal;
            ramUsedBytes = memTotal - memAvailable;
        }
#endif
    };
}
