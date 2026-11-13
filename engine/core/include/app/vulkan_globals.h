#pragma once

#include "../common_defines.h"

namespace engine
{
		//#define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
		static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
#endif



		static uint32_t					m_apiVersion = VK_API_VERSION_1_0;


		// Data
		static VkAllocationCallbacks*	g_Allocator = nullptr;
		static VkInstance               g_Instance = VK_NULL_HANDLE;
		static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
		static VkDevice                 g_Device = VK_NULL_HANDLE;
		static uint32_t                 g_QueueFamily = (uint32_t)-1;
		static VkQueue                  g_Queue = VK_NULL_HANDLE;
		static VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
		static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

		static ImGui_ImplVulkanH_Window g_MainWindowData;
		static uint32_t                 g_MinImageCount = 2;
		static bool                     g_SwapChainRebuild = false;
}