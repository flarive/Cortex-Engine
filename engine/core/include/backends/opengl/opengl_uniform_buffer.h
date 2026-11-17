#pragma once

#include "../../common_defines.h"

namespace engine
{
    struct OpenGLUniformBuffer final
    {
        GLuint id = 0;
        GLuint binding = 0;
        size_t size = 0;
    };
}