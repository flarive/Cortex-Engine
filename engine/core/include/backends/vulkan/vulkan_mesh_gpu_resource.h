#pragma once

#include "../mesh_gpu_resource.h"

#include "../../common_defines.h"

namespace engine
{
    class VulkanMeshGPUResource final : public MeshGPUResource
    {
    public:
        VkBuffer vertexBuffer{};
        VkBuffer indexBuffer{};

        VkDeviceMemory vertexMemory{};
        VkDeviceMemory indexMemory{};
    };
}