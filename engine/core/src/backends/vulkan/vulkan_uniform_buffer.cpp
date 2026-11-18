#include "../../../include/backends/vulkan/vulkan_uniform_buffer.h"

#include "../../../include/backends/vulkan/vulkan_helper.h"

engine::VulkanUniformBuffer::VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t binding, VkDeviceSize size)
    : m_device(device), m_physicalDevice(physicalDevice), m_binding(binding), m_size(size)
{
    VulkanHelper::createBuffer(m_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_buffer, m_memory);
}
//
//void engine::VulkanUniformBuffer::createBuffer()
//{
//    VkBufferCreateInfo bufferInfo{};
//    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//    bufferInfo.size = m_size;
//    bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
//    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//    vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_buffer);
//
//    VkMemoryRequirements memRequirements;
//
//    vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);
//
//    VkMemoryAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//
//    allocInfo.allocationSize = memRequirements.size;
//
//    allocInfo.memoryTypeIndex = findMemoryType(
//            memRequirements.memoryTypeBits,
//            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//    vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory);
//
//    vkBindBufferMemory(m_device, m_buffer, m_memory, 0);
//}
//
//uint32_t engine::VulkanUniformBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
//{
//    VkPhysicalDeviceMemoryProperties memProperties;
//
//    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);
//
//    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
//    {
//        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
//        {
//            return i;
//        }
//    }
//
//    throw std::runtime_error("Failed to find suitable memory type.");
//}

void engine::VulkanUniformBuffer::update(const void* data, size_t size)
{
    void* mappedMemory{};

    vkMapMemory(m_device, m_memory, 0, size, 0, &mappedMemory);

    memcpy(mappedMemory, data, size);

    vkUnmapMemory(m_device, m_memory);
}

engine::VulkanUniformBuffer::~VulkanUniformBuffer()
{
    if (m_buffer)
        vkDestroyBuffer(m_device, m_buffer, nullptr);

    if (m_memory)
        vkFreeMemory(m_device, m_memory, nullptr);
}