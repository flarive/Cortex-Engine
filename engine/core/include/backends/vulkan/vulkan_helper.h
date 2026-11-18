#pragma once

#include "../../common_defines.h"

namespace engine
{
	class VulkanHelper final
	{
	public:
		static void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,	VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	private:
		static uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};
}