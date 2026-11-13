#pragma once

#include "../common_defines.h"

namespace engine
{
		//#define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
	extern VkDebugReportCallbackEXT g_DebugReport;
#endif



		extern uint32_t					m_apiVersion;


		// Data
		extern VkAllocationCallbacks*	g_Allocator;
		extern VkInstance               g_Instance;
		extern VkPhysicalDevice         g_PhysicalDevice;
		extern VkDevice                 g_Device;
		extern uint32_t                 g_QueueFamily;
		extern VkQueue                  g_Queue;
		extern VkPipelineCache          g_PipelineCache;
		extern VkDescriptorPool         g_DescriptorPool;

		extern ImGui_ImplVulkanH_Window g_MainWindowData;
		extern uint32_t                 g_MinImageCount;
		extern bool                     g_SwapChainRebuild;
}