#pragma once

#include <windows.h>
#include <pdh.h>
#include <iostream>
#include <tchar.h>

#pragma comment(lib, "pdh.lib")

namespace engine
{
    class Test
    {
	public:
		Test() = default;
		~Test() = default;

		void run() {
			std::wstring counterPath = L"\\Processor Information(_Total)\\% Processor Utility";
			if (TestPDHCounter(counterPath)) {
				std::wcout << L"Counter is available: " << counterPath << std::endl;
			}
			else {
				std::wcout << L"Counter is NOT available: " << counterPath << std::endl;
			}
		}

    private:

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

            std::wcerr << L"PDH Error in " << context << L": " << status << L" (" << errorMsg << L")" << std::endl;
            LocalFree(errorMsg);
        }

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

    };

    

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

