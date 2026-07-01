#pragma once

#include <cstdint>

namespace engine
{
    enum class VramBackend {
        None,
        GL_NVX,
        GL_ATI,
        DXGI,
        NvidiaProc
    };

    struct VramInfo {
        std::uint64_t totalBytes;   // total VRAM
        std::uint64_t usedBytes;    // estimated used VRAM (or 0 if unknown)
        std::uint64_t freeBytes;    // estimated free VRAM (or 0 if unknown)
    };

    class VramManager
    {
    public:
        bool init();
        VramInfo query();

    private:
        VramBackend backend = VramBackend::None;

        VramInfo queryGLNVX();
        VramInfo queryGLATI();
        VramInfo queryDXGI();
        VramInfo queryNvidiaProc();
    };
}