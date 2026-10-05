#pragma once

#include "../common_defines.h"
#include <cstdint>
#include <chrono>
#include <algorithm>
#include <string>


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
        SystemMonitor();
        ~SystemMonitor() = default;
        
#if defined(_WIN32)
        void initVendor();
#endif
        

        // Call every ~200ms
        void update();


        void updateVendor();


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

        bool m_isNvidia = false;
        bool m_isAmd = false;
        bool m_isIntel = false;
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
        double getCPUProcessUsedPDH();
        double getProcessCPU();



        

		void getNvidiaGPUInfo();



#if defined(_WIN32)

        uint64_t prevIdle = 0;
        uint64_t prevTotal = 0;

        static uint64_t fileTimeToUint64(const FILETIME& ft);

        void initWindowsCPU();
        void updateWindows();

#else
        uint64_t prevIdle = 0, prevTotal = 0;

        static void readProcStat(uint64_t& idle, uint64_t& total);

        void updateLinux();
     
#endif
    };
}
