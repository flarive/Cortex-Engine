#pragma once

#include "../../common_defines.h"

namespace engine
{
    class VulkanUniformBuffer final
    {
    public:

        VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t binding, VkDeviceSize size);
        ~VulkanUniformBuffer();

        void update(const void* data, size_t size);

        VkBuffer getBuffer() const { return m_buffer; }

        VkDescriptorBufferInfo getDescriptorInfo() const
        {
            VkDescriptorBufferInfo info{};
            info.buffer = m_buffer;
            info.offset = 0;
            info.range = m_size;
            return info;
        }

        uint32_t getBinding() const
        {
            return m_binding;
        }

    private:

        //uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        //void createBuffer();

    private:

        VkDevice m_device{};
        VkPhysicalDevice m_physicalDevice{};

        VkBuffer m_buffer{};
        VkDeviceMemory m_memory{};

        uint32_t m_binding{};
        VkDeviceSize m_size{};
    };
}
