#pragma once

#include <windows.h>
#include <chrono>

namespace engine
{
    class CpuUsage
    {
    public:
        CpuUsage()
        {
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            m_numProcessors = si.dwNumberOfProcessors;

            m_lastWallTime = std::chrono::steady_clock::now();
            m_lastCpuTime = GetProcessCpuTime();
        }

        double GetUsagePercent()
        {
            auto nowWall = std::chrono::steady_clock::now();
            uint64_t nowCpu = GetProcessCpuTime();

            double elapsedWall =
                std::chrono::duration<double>(nowWall - m_lastWallTime).count();

            double elapsedCpu =
                double(nowCpu - m_lastCpuTime) / 10000000.0; // FILETIME -> seconds

            m_lastWallTime = nowWall;
            m_lastCpuTime = nowCpu;

            return (elapsedCpu / elapsedWall) * 100.0 / m_numProcessors;
        }

    private:
        uint64_t GetProcessCpuTime()
        {
            FILETIME create, exit, kernel, user;
            GetProcessTimes(
                GetCurrentProcess(),
                &create, &exit,
                &kernel, &user);

            ULARGE_INTEGER k, u;
            k.LowPart = kernel.dwLowDateTime;
            k.HighPart = kernel.dwHighDateTime;

            u.LowPart = user.dwLowDateTime;
            u.HighPart = user.dwHighDateTime;

            return k.QuadPart + u.QuadPart;
        }

        uint32_t m_numProcessors;
        uint64_t m_lastCpuTime;
        std::chrono::steady_clock::time_point m_lastWallTime;
    };
}

