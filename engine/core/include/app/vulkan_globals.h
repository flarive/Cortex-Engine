#pragma once

#include "../common_defines.h"

namespace engine
{
    class VulkanGlobals final
    {
    public:

//#ifdef _DEBUG
//#ifdef APP_USE_VULKAN_DEBUG_REPORT
        static VkDebugReportCallbackEXT g_DebugReport;
//#endif
//#endif


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

#ifdef _DEBUG
#ifdef APP_USE_VULKAN_DEBUG_REPORT
        static VkDebugReportCallbackEXT getDebugReport();
#endif
#endif
    };
}