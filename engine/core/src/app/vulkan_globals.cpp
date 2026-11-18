#include "../../include/app/vulkan_globals.h"
//
//namespace engine
//{
//	//#define APP_USE_UNLIMITED_FRAME_RATE
//#ifdef _DEBUG
//#define APP_USE_VULKAN_DEBUG_REPORT
//	VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
//#endif
//
//
//
//	uint32_t					m_apiVersion = VK_API_VERSION_1_0;
//
//
//	// Data
//	VkAllocationCallbacks*	g_Allocator = nullptr;
//	VkInstance               g_Instance = VK_NULL_HANDLE;
//	VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
//	VkDevice                 g_Device = VK_NULL_HANDLE;
//	uint32_t                 g_QueueFamily = (uint32_t)-1;
//	VkQueue                  g_Queue = VK_NULL_HANDLE;
//	VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
//	VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;
//
//	ImGui_ImplVulkanH_Window g_MainWindowData;
//	uint32_t                 g_MinImageCount = 2;
//	bool                     g_SwapChainRebuild = false;
//}


namespace engine
{
#ifdef _DEBUG
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    VkDebugReportCallbackEXT VulkanGlobals::g_DebugReport = VK_NULL_HANDLE;
#endif
#endif
    
    
    VkDevice VulkanGlobals::g_Device = VK_NULL_HANDLE;
    VkInstance VulkanGlobals::g_Instance = VK_NULL_HANDLE;
    VkPhysicalDevice VulkanGlobals::g_PhysicalDevice = VK_NULL_HANDLE;

    uint32_t VulkanGlobals::g_ApiVersion = VK_API_VERSION_1_0;

    VkAllocationCallbacks* VulkanGlobals::g_Allocator = nullptr;

    VkQueue VulkanGlobals::g_Queue = VK_NULL_HANDLE;
    uint32_t VulkanGlobals::g_QueueFamily = 0;

    VkPipelineCache VulkanGlobals::g_PipelineCache = VK_NULL_HANDLE;
    VkDescriptorPool VulkanGlobals::g_DescriptorPool = VK_NULL_HANDLE;

    ImGui_ImplVulkanH_Window VulkanGlobals::g_MainWindowData{};
    uint32_t VulkanGlobals::g_MinImageCount = 2;
    bool VulkanGlobals::g_SwapChainRebuild = false;
    
    VkInstance VulkanGlobals::getInstance()
    {
        return VulkanGlobals::g_Instance;
    }

    VkPhysicalDevice VulkanGlobals::getPhysicalDevice()
    {
        return VulkanGlobals::g_PhysicalDevice;
    }

    VkDevice VulkanGlobals::getDevice()
    {
        return VulkanGlobals::g_Device;
    }

    VkQueue VulkanGlobals::getQueue()
    {
        return VulkanGlobals::g_Queue;
    }

    uint32_t VulkanGlobals::getQueueFamily()
    {
        return VulkanGlobals::g_QueueFamily;
    }

    VkDescriptorPool VulkanGlobals::getDescriptorPool()
    {
        return VulkanGlobals::g_DescriptorPool;
    }

    VkPipelineCache VulkanGlobals::getPipelineCache()
    {
        return VulkanGlobals::g_PipelineCache;
    }

    uint32_t VulkanGlobals::getApiVersion()
    {
        return VulkanGlobals::g_ApiVersion;
    }

    ImGui_ImplVulkanH_Window VulkanGlobals::getMainWindowData()
    {
        return VulkanGlobals::g_MainWindowData;
    }

    uint32_t VulkanGlobals::getMinImageCount()
    {
        return VulkanGlobals::g_MinImageCount;
    }

    bool VulkanGlobals::getSwapChainRebuild()
    {
        return VulkanGlobals::g_SwapChainRebuild;
    }
}