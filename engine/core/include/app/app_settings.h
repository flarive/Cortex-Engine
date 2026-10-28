#pragma once

#include <string>

#include "../common_defines.h"

namespace engine
{
    enum class BackendType
    {
        OpenGL = 0,
        Vulkan = 1
    };

    struct AppSettings final
    {
        unsigned int targetFPS{ 0 }; // 0 means no FPS capping
        BackendType backend{ BackendType::OpenGL };
    };
}