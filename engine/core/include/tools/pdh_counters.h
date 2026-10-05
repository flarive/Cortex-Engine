#pragma once

#if defined(_WIN32)

//#include <windows.h>
#include <pdh.h>
#include <iostream>
#include <tchar.h>
#include <pdhmsg.h>
#include <vector>
#include <string>



//#include <windows.h>
//#include <pdh.h>
//#include <psapi.h>

#include <thread>
#include <atomic>
#include <chrono>


#pragma comment(lib, "pdh.lib")

#endif



namespace engine
{
    class PDHCounters final
    {
	public:
        PDHCounters();
		~PDHCounters();

    #if defined(_WIN32)
        bool testPDHCounter(const std::wstring& counterPath);

        void listAll();


        double getCounterValue(const std::wstring& counterPath);


        void initTotalUsedCPUQuery();

        double getTotalUsedCPUQueryValue();

        void initProcessUsedCPUQuery();

        double getProcessUsedCPUQueryValue();
    #endif


    private:
    #if defined(_WIN32)

        PDH_HQUERY m_totalCpuUsedQuery = nullptr;
        PDH_HCOUNTER m_totalCpuUsedCounter = nullptr;
        bool m_totalCpuUsedInitialized = false;


        PDH_HQUERY m_processCpuUsedQuery = nullptr;
        PDH_HCOUNTER m_processCpuUsedCounter = nullptr;
        bool m_processCpuUsedInitialized = false;

        //const std::wstring TOTAL_CPU_USED_COUNTER_PATH = L"\\Informations sur le processeur(_Total)\\Pourcentage de rendement du processeur";
        const std::wstring TOTAL_CPU_USED_COUNTER_PATH = L"\\Processeur(_Total)\\% temps processeur";
        const std::wstring PROCESS_CPU_USED_COUNTER_PATH = L"\\Processus(App)\\% temps processeur";

		const unsigned int SLEEP_DURATION_MS = 100; // 100 milliseconds


        std::thread m_updateThread;
        std::atomic<bool> m_running = false;

        std::atomic<double> m_totalCpuValue = 0.0;
        std::atomic<double> m_processCpuValue = 0.0;

        void updateLoop();

        

        void handlePDHError(PDH_STATUS status, const std::wstring& context);

        

        void listProcessorCounters(const wchar_t* objectName);
        
    #endif
    };
}