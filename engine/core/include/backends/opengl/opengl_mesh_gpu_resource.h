#pragma once

#include "../mesh_gpu_resource.h"

#include "../../common_defines.h"

namespace engine
{
    class OpenGLMeshGPUResource final : public MeshGPUResource
    {
    public:
        GLuint vao{};
        GLuint vbo{};
        GLuint ebo{};
    };
}
