#pragma once

#if defined(_WIN32)

//#include <windows.h>
#include <pdh.h>
//#include <iostream>
#include <tchar.h>
#include <pdhmsg.h>
//#include <vector>
//#include <string>



//#include <windows.h>
//#include <pdh.h>
//#include <psapi.h>



#pragma comment(lib, "pdh.lib")

#endif

namespace engine
{
    class PDHCounters final
    {
	public:
        PDHCounters() = default;
		~PDHCounters() = default;

        #if defined(_WIN32)

        bool TestPDHCounter(const std::wstring& counterPath) {
            PDH_HQUERY query;
            PDH_HCOUNTER counter;
            PDH_STATUS status;

            status = PdhOpenQuery(NULL, 0, &query);
            if (status != ERROR_SUCCESS) {
                HandlePDHError(status, L"PdhOpenQuery");
                return false;
            }

            status = PdhAddCounter(query, counterPath.c_str(), 0, &counter);
            if (status != ERROR_SUCCESS) {
                HandlePDHError(status, L"PdhAddCounter");
                PdhCloseQuery(query);
                return false;
            }

            PdhRemoveCounter(counter);
            PdhCloseQuery(query);
            return true;
        }
        

        void listAll()
        {
            listProcessorCounters(L"Processeur");
            listProcessorCounters(L"Informations sur le processeur");
        }

        double getCounterValue(const std::wstring& counterPath)
        {
            double val = 0.0;

            PDH_HQUERY query;
            PDH_HCOUNTER counter;
            PDH_STATUS status;

            status = PdhOpenQuery(NULL, 0, &query);
            if (status != ERROR_SUCCESS) {
                HandlePDHError(status, L"PdhOpenQuery");
                return false;
            }

            status = PdhAddCounter(query, counterPath.c_str(), 0, &counter);
            if (status == ERROR_SUCCESS)
            {
                PdhCollectQueryData(query);
                Sleep(100);
                PdhCollectQueryData(query);

                PDH_FMT_COUNTERVALUE value{};
                PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value);

                val = value.doubleValue;
            }

            return val;
        }



        void initCPUQuery()
        {
            if (m_cpuInitialized)
                return;

            if (PdhOpenQuery(NULL, 0, &m_cpuQuery) != ERROR_SUCCESS)
                return;

            if (PdhAddCounterW(m_cpuQuery, COUNTER_PATH.c_str(), 0, &m_cpuCounter) != ERROR_SUCCESS)
                return;

            // Required double sample
            PdhCollectQueryData(m_cpuQuery);
            Sleep(100);
            PdhCollectQueryData(m_cpuQuery);

            m_cpuInitialized = true;
        }

        double getCPUQueryValue()
        {
            double cpu = 0.0;
            
            if (!m_cpuInitialized)
                initCPUQuery();

            PdhCollectQueryData(m_cpuQuery);

            PDH_FMT_COUNTERVALUE value{};
            if (PdhGetFormattedCounterValue(m_cpuCounter, PDH_FMT_DOUBLE, NULL, &value) == ERROR_SUCCESS)
            {
                cpu = value.doubleValue;

                SYSTEM_INFO info;
                GetSystemInfo(&info);

                // Task Manager clamps
                if (cpu < 0.0) cpu = 0.0;
                if (cpu > 100.0) cpu = 100.0;
            }

            return cpu;
        }
    #endif


    private:
    #if defined(_WIN32)

        PDH_HQUERY m_cpuQuery = nullptr;
        PDH_HCOUNTER m_cpuCounter = nullptr;
        bool m_cpuInitialized = false;

        const std::wstring COUNTER_PATH = L"\\Informations sur le processeur(_Total)\\Pourcentage de rendement du processeur";
        

        void HandlePDHError(PDH_STATUS status, const std::wstring& context) {
            if (status == ERROR_SUCCESS) return;

            LPWSTR errorMsg = nullptr;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
                GetModuleHandle(L"pdh.dll"),
                status,
                0,
                (LPWSTR)&errorMsg,
                0,
                NULL
            );

            //L’objet spécifié n’a pas été trouvé sur l’ordinateur.
            std::wcerr << L"PDH Error in " << context << L": " << status << L" (" << errorMsg << L")" << std::endl;
            LocalFree(errorMsg);
        }

        

        void listProcessorCounters(const wchar_t* objectName)
        {
            DWORD counterListSize = 0;
            DWORD instanceListSize = 0;

            // First call: get required buffer sizes
            PdhEnumObjectItemsW(
                NULL, NULL,
                objectName,
                NULL, &counterListSize,
                NULL, &instanceListSize,
                PERF_DETAIL_WIZARD,
                0
            );

            std::vector<wchar_t> counterList(counterListSize);
            std::vector<wchar_t> instanceList(instanceListSize);

            // Second call: retrieve counters + instances
            PDH_STATUS status = PdhEnumObjectItemsW(
                NULL, NULL,
                objectName,
                counterList.data(), &counterListSize,
                instanceList.data(), &instanceListSize,
                PERF_DETAIL_WIZARD,
                0
            );

            if (status != ERROR_SUCCESS)
            {
                std::wcerr << L"Failed: " << status << std::endl;
                return;
            }

            std::wcout << L"\n=== Object: " << objectName << L" ===\n";

            // Print counters
            std::wcout << L"\nCounters:\n";
            for (wchar_t* c = counterList.data(); *c; c += wcslen(c) + 1)
                std::wcout << L"  " << c << std::endl;

            // Print instances
            std::wcout << L"\nInstances:\n";
            for (wchar_t* i = instanceList.data(); *i; i += wcslen(i) + 1)
                std::wcout << L"  " << i << std::endl;
        }
    #endif
    };
}