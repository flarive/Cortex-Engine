#include "../../include/tools/vram_manager.h"

#include <glad/glad.h>

// for LINUX
#include <fstream>
#include <string>

// windows os (any vendor)
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")

bool engine::VramManager::init()
{
    // Prefer OpenGL vendor extensions if you already have a GL context.

    if (GLAD_GL_NVX_gpu_memory_info) {
        backend = VramBackend::GL_NVX;
        return true;
    }

    if (GLAD_GL_ATI_meminfo) {
        backend = VramBackend::GL_ATI;
        return true;
    }

#ifdef _WIN32
    backend = VramBackend::DXGI;
    return true;
#else
    // Optional: check for /proc/driver/nvidia
    std::ifstream f("/proc/driver/nvidia/gpus/0/information");
    if (f.is_open()) {
        backend = VramBackend::NvidiaProc;
        return true;
    }
#endif

    backend = VramBackend::None;
    return false;
}

engine::VramInfo engine::VramManager::query()
{
    switch (backend) {
    case VramBackend::GL_NVX:      return queryGLNVX();
    case VramBackend::GL_ATI:      return queryGLATI();
    case VramBackend::DXGI:        return queryDXGI();
    case VramBackend::NvidiaProc:  return queryNvidiaProc();
    default:                       return VramInfo{};
    }
}

/// <summary>
/// Query NVIDIA vendor
/// </summary>
/// <returns></returns>
engine::VramInfo engine::VramManager::queryGLNVX()
{
    engine::VramInfo info{};

    GLint totalKB = 0;
    GLint curAvailKB = 0;

    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalKB);
    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &curAvailKB);

    info.totalBytes = static_cast<std::uint64_t>(totalKB) * 1024;
    info.freeBytes = static_cast<std::uint64_t>(curAvailKB) * 1024;
    info.usedBytes = info.totalBytes - info.freeBytes;

    return info;
}

/// <summary>
/// Query ATI/AMD vendor
/// </summary>
/// <returns></returns>
engine::VramInfo engine::VramManager::queryGLATI()
{
    engine::VramInfo info{};

    GLint texFree[4] = { 0 };
    glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, texFree);

    // texFree[0] = total free texture memory in KB (approx)
    info.freeBytes = static_cast<std::uint64_t>(texFree[0]) * 1024;
    // totalBytes is not directly exposed; you can leave it 0 or estimate.
    info.totalBytes = 0;
    info.usedBytes = 0;

    return info;
}

/// <summary>
/// Query Windows (fallback)
/// </summary>
/// <returns></returns>
engine::VramInfo engine::VramManager::queryDXGI()
{
    engine::VramInfo info{};

    IDXGIFactory* factory = nullptr;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
        return info;

    IDXGIAdapter* adapter = nullptr;
    if (FAILED(factory->EnumAdapters(0, &adapter))) {
        factory->Release();
        return info;
    }

    DXGI_ADAPTER_DESC desc;
    adapter->GetDesc(&desc);

    info.totalBytes = static_cast<std::uint64_t>(desc.DedicatedVideoMemory);
    // DXGI doesn’t give live usage; leave used/free as 0.

    adapter->Release();
    factory->Release();

    return info;
}

engine::VramInfo engine::VramManager::queryNvidiaProc()
{
    engine::VramInfo info{};
    std::ifstream f("/proc/driver/nvidia/gpus/0/information");
    if (!f.is_open())
        return info;

    std::string line;
    while (std::getline(f, line)) {
        auto pos = line.find("FB Memory");
        if (pos != std::string::npos) {
            // Example: "FB Memory Usage     : 4096 MiB"
            auto mibPos = line.find("MiB");
            if (mibPos != std::string::npos) {
                std::string num = line.substr(pos + 15, mibPos - (pos + 15));
                std::uint64_t mib = std::stoull(num);
                info.totalBytes = mib * 1024ull * 1024ull;
            }
        }
    }
    return info;
}
