#include "../../include/tools/pdh_counters.h"

#include <algorithm>

engine::PDHCounters::PDHCounters()
{
    initTotalUsedCPUQuery();
    initProcessUsedCPUQuery();

    m_running = true;

    m_updateThread = std::thread(
        &PDHCounters::updateLoop,
        this
    );
}

bool engine::PDHCounters::testPDHCounter(const std::wstring& counterPath) {
    PDH_HQUERY query;
    PDH_HCOUNTER counter;
    PDH_STATUS status;

    status = PdhOpenQuery(NULL, 0, &query);
    if (status != ERROR_SUCCESS) {
        handlePDHError(status, L"PdhOpenQuery");
        return false;
    }

    status = PdhAddCounter(query, counterPath.c_str(), 0, &counter);
    if (status != ERROR_SUCCESS) {
        handlePDHError(status, L"PdhAddCounter");
        PdhCloseQuery(query);
        return false;
    }

    PdhRemoveCounter(counter);
    PdhCloseQuery(query);
    return true;
}

void engine::PDHCounters::listAll()
{
    listProcessorCounters(L"Processeur");
    listProcessorCounters(L"Informations sur le processeur");
}

double engine::PDHCounters::getCounterValue(const std::wstring& counterPath)
{
    double val = 0.0;

    PDH_HQUERY query;
    PDH_HCOUNTER counter;
    PDH_STATUS status;

    status = PdhOpenQuery(NULL, 0, &query);
    if (status != ERROR_SUCCESS) {
        handlePDHError(status, L"PdhOpenQuery");
        return false;
    }

    status = PdhAddCounter(query, counterPath.c_str(), 0, &counter);
    if (status == ERROR_SUCCESS)
    {
        PdhCollectQueryData(query);
        Sleep(SLEEP_DURATION_MS);
        PdhCollectQueryData(query);

        PDH_FMT_COUNTERVALUE value{};
        PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value);

        val = value.doubleValue;
    }

    return val;
}

void engine::PDHCounters::initTotalUsedCPUQuery()
{
    if (m_totalCpuUsedInitialized)
        return;

    if (PdhOpenQuery(NULL, 0, &m_totalCpuUsedQuery) != ERROR_SUCCESS)
        return;

    if (PdhAddCounterW(m_totalCpuUsedQuery, TOTAL_CPU_USED_COUNTER_PATH.c_str(), 0, &m_totalCpuUsedCounter) != ERROR_SUCCESS)
        return;

    // Required double sample
    PdhCollectQueryData(m_totalCpuUsedQuery);
    Sleep(SLEEP_DURATION_MS);
    PdhCollectQueryData(m_totalCpuUsedQuery);

    m_totalCpuUsedInitialized = true;
}

double engine::PDHCounters::getTotalUsedCPUQueryValue()
{
    return m_totalCpuValue.load(
        std::memory_order_relaxed
    );
}

void engine::PDHCounters::initProcessUsedCPUQuery()
{
    if (m_processCpuUsedInitialized)
        return;

    if (PdhOpenQuery(NULL, 0, &m_processCpuUsedQuery) != ERROR_SUCCESS)
        return;

    if (PdhAddCounterW(m_processCpuUsedQuery, PROCESS_CPU_USED_COUNTER_PATH.c_str(), 0, &m_processCpuUsedCounter) != ERROR_SUCCESS)
        return;

    // Required double sample
    PdhCollectQueryData(m_processCpuUsedQuery);
    Sleep(SLEEP_DURATION_MS);
    PdhCollectQueryData(m_processCpuUsedQuery);

    m_processCpuUsedInitialized = true;
}

double engine::PDHCounters::getProcessUsedCPUQueryValue()
{
    return m_processCpuValue.load(
        std::memory_order_relaxed
    );
}

void engine::PDHCounters::listProcessorCounters(const wchar_t* objectName)
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

void engine::PDHCounters::updateLoop()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    while (m_running)
    {
        //----------------------------------
        // Total CPU
        //----------------------------------

        if (m_totalCpuUsedInitialized)
        {
            if (PdhCollectQueryData(m_totalCpuUsedQuery) == ERROR_SUCCESS)
            {
                PDH_FMT_COUNTERVALUE value{};

                if (PdhGetFormattedCounterValue(
                    m_totalCpuUsedCounter,
                    PDH_FMT_DOUBLE,
                    nullptr,
                    &value) == ERROR_SUCCESS)
                {
                    double cpu = value.doubleValue;

                    cpu = std::clamp(cpu, 0.0, 100.0);

                    m_totalCpuValue.store(
                        cpu,
                        std::memory_order_relaxed
                    );
                }
            }
        }

        //----------------------------------
        // Process CPU
        //----------------------------------

        if (m_processCpuUsedInitialized)
        {
            if (PdhCollectQueryData(m_processCpuUsedQuery) == ERROR_SUCCESS)
            {
                PDH_FMT_COUNTERVALUE value{};

                if (PdhGetFormattedCounterValue(
                    m_processCpuUsedCounter,
                    PDH_FMT_DOUBLE,
                    nullptr,
                    &value) == ERROR_SUCCESS)
                {
                    double cpu = value.doubleValue;

                    cpu /= info.dwNumberOfProcessors;

                    cpu = std::clamp(cpu, 0.0, 100.0);

                    m_processCpuValue.store(
                        cpu,
                        std::memory_order_relaxed
                    );
                }
            }
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }
}

void engine::PDHCounters::handlePDHError(PDH_STATUS status, const std::wstring& context)
{
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

    // Specified object not found on this computer
    std::wcerr << L"PDH Error in " << context << L": " << status << L" (" << errorMsg << L")" << std::endl;
    LocalFree(errorMsg);
}

engine::PDHCounters::~PDHCounters()
{
    m_running = false;

    if (m_updateThread.joinable())
        m_updateThread.join();

    if (m_totalCpuUsedCounter)
        PdhRemoveCounter(m_totalCpuUsedCounter);

    if (m_totalCpuUsedQuery)
        PdhCloseQuery(m_totalCpuUsedQuery);

    if (m_processCpuUsedCounter)
        PdhRemoveCounter(m_processCpuUsedCounter);

    if (m_processCpuUsedQuery)
        PdhCloseQuery(m_processCpuUsedQuery);
}