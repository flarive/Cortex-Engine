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

#include "nvidia_monitor.h"


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

            updateVendor();
        }


        void updateVendor()
        {
            getNvidiaGPUInfo();
        }


        double getCPU() const { return m_cpuTotalUsedPercent; }
        double getCPUProcess() const { return m_cpuProcessPercent; }


        uint64_t getRAMUsed() const { return m_ramUsedBytes; }
        uint64_t getRAMTotal() const { return m_ramTotalBytes; }
        uint64_t getProcessRAM();
        

		int getVendorGPUUsage() const { return m_vendorGPUUsage; }
		double getVendorGPUUsagePercent() const { return m_vendorGPUUsagePercent; }
		int getVendorTemperature() const { return m_vendorTemperature; }
		double getVendorPowerUsageWatts() const { return m_vendorPowerUsageWatts; }


        std::string GetGPUVendor();
        std::string GetGPURenderer();
        std::string GetGPUVersion();




    private:
        
        #if defined(_WIN32)
        PDHCounters* m_PDHCounters{};
        #endif

        NvidiaGpuMonitor m_nvidiaMonitor{};

        
        double m_cpuTotalUsedPercent = 0.0;
        double m_cpuProcessPercent = 0.0;
        uint64_t m_ramUsedBytes = 0;
        uint64_t m_ramTotalBytes = 0;


        int m_vendorGPUUsage = 0;
		double m_vendorGPUUsagePercent = 0.0;
		int m_vendorTemperature = 0;
		double m_vendorPowerUsageWatts = 0.0;


        double getCPUTotalUsed();
        double getCPUTotalUsedPDH();
        double getProcessCPU();



        

		void getNvidiaGPUInfo();


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
            m_cpuTotalUsedPercent = getCPUTotalUsedPDH(); // using Windows PDH (closest to Task manager value)
            #else
            m_cpuTotalUsedPercent = getCPUTotalUsed();
            #endif

            m_cpuProcessPercent = getProcessCPU();

            // RAM
            MEMORYSTATUSEX mem{};
            mem.dwLength = sizeof(mem);
            GlobalMemoryStatusEx(&mem);

            m_ramTotalBytes = mem.ullTotalPhys;
            m_ramUsedBytes = mem.ullTotalPhys - mem.ullAvailPhys;
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
