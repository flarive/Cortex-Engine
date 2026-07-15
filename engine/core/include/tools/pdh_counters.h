#pragma once

#include <windows.h>
#include <pdh.h>
#include <iostream>
#include <tchar.h>



#include <pdhmsg.h>
#include <vector>
#include <string>


#pragma comment(lib, "pdh.lib")

namespace engine
{
    class PDHCounters final
    {
	public:
        PDHCounters() = default;
		~PDHCounters() = default;

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



        void initPDHCPU()
        {
            if (cpuInitialized)
                return;

            if (PdhOpenQuery(NULL, 0, &cpuQuery) != ERROR_SUCCESS)
                return;

            if (PdhAddCounterW(cpuQuery, COUNTER_PATH.c_str(), 0, &cpuCounter) != ERROR_SUCCESS)
                return;

            // Required double sample
            PdhCollectQueryData(cpuQuery);
            Sleep(100);
            PdhCollectQueryData(cpuQuery);

            cpuInitialized = true;
        }

        double getCPUTotalUsedPDH()
        {
            if (!cpuInitialized)
                initPDHCPU();

            PdhCollectQueryData(cpuQuery);

            PDH_FMT_COUNTERVALUE value{};
            if (PdhGetFormattedCounterValue(cpuCounter, PDH_FMT_DOUBLE, NULL, &value) == ERROR_SUCCESS)
                return value.doubleValue;

            return 0.0;
        }

    protected:
        const std::wstring COUNTER_PATH = L"\\Informations sur le processeur(_Total)\\% temps utilisateur";


    private:

        PDH_HQUERY cpuQuery = nullptr;
        PDH_HCOUNTER cpuCounter = nullptr;
        bool cpuInitialized = false;

        
        //L"\\Informations sur le processeur(_Total)\\Pourcentage de rendement du processeur",

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


        
    };




    

    //int main()
    //{
    //    listProcessorCounters(L"Processor");
    //    listProcessorCounters(L"Processor Information");
    //    return 0;
    //}

    

    //int main() {
    //    std::wstring counterPath = L"\\Processor(_Total)\\% Processor Time";
    //    if (TestPDHCounter(counterPath)) {
    //        std::wcout << L"Counter is available: " << counterPath << std::endl;
    //    }
    //    else {
    //        std::wcout << L"Counter is NOT available: " << counterPath << std::endl;
    //    }

    //    return 0;
    //}
}

