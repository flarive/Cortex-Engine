#pragma once
#include <windows.h>
#include <string>
#include <stdexcept>

namespace engine
{
    // Minimal NVML type definitions (subset of nvml.h)

    typedef int nvmlReturn_t;

    static const nvmlReturn_t NVML_SUCCESS = 0;

    typedef struct nvmlDevice_st* nvmlDevice_t;

    typedef struct nvmlUtilization_st
    {
        unsigned int gpu;    // percent
        unsigned int memory; // percent
    } nvmlUtilization_t;

    typedef struct nvmlMemory_st
    {
        unsigned long long total; // bytes
        unsigned long long free;  // bytes
        unsigned long long used;  // bytes
    } nvmlMemory_t;

    // Temperature sensor type
    static const int NVML_TEMPERATURE_GPU = 0;

    // Function pointer typedefs (subset of NVML API)

    typedef nvmlReturn_t(*nvmlInit_t)();
    typedef nvmlReturn_t(*nvmlShutdown_t)();
    typedef nvmlReturn_t(*nvmlDeviceGetHandleByIndex_t)(unsigned int index, nvmlDevice_t* device);
    typedef nvmlReturn_t(*nvmlDeviceGetUtilizationRates_t)(nvmlDevice_t device, nvmlUtilization_t* utilization);
    typedef nvmlReturn_t(*nvmlDeviceGetMemoryInfo_t)(nvmlDevice_t device, nvmlMemory_t* memory);
    typedef nvmlReturn_t(*nvmlDeviceGetTemperature_t)(nvmlDevice_t device, int sensorType, unsigned int* temp);
    typedef nvmlReturn_t(*nvmlDeviceGetPowerUsage_t)(nvmlDevice_t device, unsigned int* power);

    // Header-only dynamic NVML loader + monitor

    class NvidiaGpuMonitor final
    {
    public:
        NvidiaGpuMonitor()
            : m_module(nullptr),
            m_available(false),
            m_device(nullptr),
            m_nvmlInit(nullptr),
            m_nvmlShutdown(nullptr),
            m_nvmlDeviceGetHandleByIndex(nullptr),
            m_nvmlDeviceGetUtilizationRates(nullptr),
            m_nvmlDeviceGetMemoryInfo(nullptr),
            m_nvmlDeviceGetTemperature(nullptr),
            m_nvmlDeviceGetPowerUsage(nullptr)
        {
            // Try to load nvml.dll from system
            m_module = ::LoadLibraryA("nvml.dll");
            if (!m_module)
            {
                // NVML not available on this system
                return;
            }

            // Resolve symbols
            m_nvmlInit = (nvmlInit_t)GetProcAddress(m_module, "nvmlInit_v2");
            if (!m_nvmlInit)
                m_nvmlInit = (nvmlInit_t)GetProcAddress(m_module, "nvmlInit");

            m_nvmlShutdown = (nvmlShutdown_t)GetProcAddress(m_module, "nvmlShutdown");
            m_nvmlDeviceGetHandleByIndex =
                (nvmlDeviceGetHandleByIndex_t)GetProcAddress(m_module, "nvmlDeviceGetHandleByIndex");
            m_nvmlDeviceGetUtilizationRates =
                (nvmlDeviceGetUtilizationRates_t)GetProcAddress(m_module, "nvmlDeviceGetUtilizationRates");
            m_nvmlDeviceGetMemoryInfo =
                (nvmlDeviceGetMemoryInfo_t)GetProcAddress(m_module, "nvmlDeviceGetMemoryInfo");
            m_nvmlDeviceGetTemperature =
                (nvmlDeviceGetTemperature_t)GetProcAddress(m_module, "nvmlDeviceGetTemperature");
            m_nvmlDeviceGetPowerUsage =
                (nvmlDeviceGetPowerUsage_t)GetProcAddress(m_module, "nvmlDeviceGetPowerUsage");

            if (!m_nvmlInit ||
                !m_nvmlShutdown ||
                !m_nvmlDeviceGetHandleByIndex ||
                !m_nvmlDeviceGetUtilizationRates ||
                !m_nvmlDeviceGetMemoryInfo ||
                !m_nvmlDeviceGetTemperature ||
                !m_nvmlDeviceGetPowerUsage)
            {
                // Missing symbols → treat as unavailable
                ::FreeLibrary(m_module);
                m_module = nullptr;
                return;
            }

            // Initialize NVML
            nvmlReturn_t res = m_nvmlInit();
            if (res != NVML_SUCCESS)
            {
                ::FreeLibrary(m_module);
                m_module = nullptr;
                return;
            }

            // Use GPU 0 by default
            res = m_nvmlDeviceGetHandleByIndex(0, &m_device);
            if (res != NVML_SUCCESS)
            {
                m_nvmlShutdown();
                ::FreeLibrary(m_module);
                m_module = nullptr;
                return;
            }

            m_available = true;
        }

        ~NvidiaGpuMonitor()
        {
            if (m_available && m_nvmlShutdown)
                m_nvmlShutdown();

            if (m_module)
                ::FreeLibrary(m_module);
        }

        bool isAvailable() const { return m_available; }

        // GPU usage (%) [0..100], or -1 if unavailable
        int getGpuUsage() const
        {
            if (!m_available)
                return -1;

            nvmlUtilization_t util{};
            nvmlReturn_t res = m_nvmlDeviceGetUtilizationRates(m_device, &util);
            if (res != NVML_SUCCESS)
                return -1;

            return static_cast<int>(util.gpu);
        }

        // Memory controller usage (%) [0..100], or -1 if unavailable
        int getMemoryControllerUsage() const
        {
            if (!m_available)
                return -1;

            nvmlUtilization_t util{};
            nvmlReturn_t res = m_nvmlDeviceGetUtilizationRates(m_device, &util);
            if (res != NVML_SUCCESS)
                return -1;

            return static_cast<int>(util.memory);
        }

        // Total VRAM (GB), or -1.0 if unavailable
        double getTotalMemoryGB() const
        {
            if (!m_available)
                return -1.0;

            nvmlMemory_t mem{};
            nvmlReturn_t res = m_nvmlDeviceGetMemoryInfo(m_device, &mem);
            if (res != NVML_SUCCESS)
                return -1.0;

            return static_cast<double>(mem.total) / (1024.0 * 1024.0 * 1024.0);
        }

        // Used VRAM (GB), or -1.0 if unavailable
        double getUsedMemoryGB() const
        {
            if (!m_available)
                return -1.0;

            nvmlMemory_t mem{};
            nvmlReturn_t res = m_nvmlDeviceGetMemoryInfo(m_device, &mem);
            if (res != NVML_SUCCESS)
                return -1.0;

            return static_cast<double>(mem.used) / (1024.0 * 1024.0 * 1024.0);
        }

        // VRAM usage percent [0..100], or -1.0 if unavailable
        double getMemoryUsagePercent() const
        {
            if (!m_available)
                return -1.0;

            nvmlMemory_t mem{};
            nvmlReturn_t res = m_nvmlDeviceGetMemoryInfo(m_device, &mem);
            if (res != NVML_SUCCESS || mem.total == 0)
                return -1.0;

            return (static_cast<double>(mem.used) / static_cast<double>(mem.total)) * 100.0;
        }

        // Temperature (°C), or -1 if unavailable
        int getTemperatureC() const
        {
            if (!m_available)
                return -1;

            unsigned int temp = 0;
            nvmlReturn_t res = m_nvmlDeviceGetTemperature(m_device, NVML_TEMPERATURE_GPU, &temp);
            if (res != NVML_SUCCESS)
                return -1;

            return static_cast<int>(temp);
        }

        // Power usage (Watts), or -1.0 if unavailable
        double getPowerUsageWatts() const
        {
            if (!m_available)
                return -1.0;

            unsigned int power = 0;
            nvmlReturn_t res = m_nvmlDeviceGetPowerUsage(m_device, &power);
            if (res != NVML_SUCCESS)
                return -1.0;

            // NVML reports milliwatts
            return static_cast<double>(power) / 1000.0;
        }

    private:
        HMODULE m_module;
        bool m_available;
        nvmlDevice_t m_device;

        nvmlInit_t                     m_nvmlInit;
        nvmlShutdown_t                 m_nvmlShutdown;
        nvmlDeviceGetHandleByIndex_t   m_nvmlDeviceGetHandleByIndex;
        nvmlDeviceGetUtilizationRates_t m_nvmlDeviceGetUtilizationRates;
        nvmlDeviceGetMemoryInfo_t      m_nvmlDeviceGetMemoryInfo;
        nvmlDeviceGetTemperature_t     m_nvmlDeviceGetTemperature;
        nvmlDeviceGetPowerUsage_t      m_nvmlDeviceGetPowerUsage;
    };
}
