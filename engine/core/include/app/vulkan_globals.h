//#pragma once
//
//#include "../common_defines.h"
//
//namespace engine
//{
//		//#define APP_USE_UNLIMITED_FRAME_RATE
//#ifdef _DEBUG
//#define APP_USE_VULKAN_DEBUG_REPORT
//	extern VkDebugReportCallbackEXT g_DebugReport;
//#endif
//
//
//
//		extern uint32_t					m_apiVersion;
//
//
//		// Data
//		extern VkAllocationCallbacks*	g_Allocator;
//		extern VkInstance               g_Instance;
//		extern VkPhysicalDevice         g_PhysicalDevice;
//		extern VkDevice                 g_Device;
//		extern uint32_t                 g_QueueFamily;
//		extern VkQueue                  g_Queue;
//		extern VkPipelineCache          g_PipelineCache;
//		extern VkDescriptorPool         g_DescriptorPool;
//
//		extern ImGui_ImplVulkanH_Window g_MainWindowData;
//		extern uint32_t                 g_MinImageCount;
//		extern bool                     g_SwapChainRebuild;
//}

#pragma once

#include "../common_defines.h"

namespace engine
{
    class VulkanGlobals final
    {
    public:

#ifdef _DEBUG
#ifdef APP_USE_VULKAN_DEBUG_REPORT
        static VkDebugReportCallbackEXT g_DebugReport;
#endif
#endif


        static uint32_t g_ApiVersion;

        static VkAllocationCallbacks* g_Allocator;
        static VkInstance g_Instance;
        static VkPhysicalDevice g_PhysicalDevice;
        static VkDevice g_Device;
        static uint32_t g_QueueFamily;
        static VkQueue g_Queue;
        static VkPipelineCache g_PipelineCache;
        static VkDescriptorPool g_DescriptorPool;

        static ImGui_ImplVulkanH_Window g_MainWindowData;
        static uint32_t g_MinImageCount;
        static bool g_SwapChainRebuild;






        static VkInstance getInstance();
        static VkPhysicalDevice getPhysicalDevice();
        static VkDevice getDevice();
        static VkQueue getQueue();
        static uint32_t getQueueFamily();

        static VkDescriptorPool getDescriptorPool();
        static VkPipelineCache getPipelineCache();

        static uint32_t getApiVersion();

        static ImGui_ImplVulkanH_Window getMainWindowData();
        static uint32_t getMinImageCount();
        static bool getSwapChainRebuild();
    };
}