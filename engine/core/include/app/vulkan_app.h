#pragma once

#include "app.h"

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

	class VulkanApp : public App
	{
	public:
		VulkanApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, const AppSettings& _settings);
		~VulkanApp() override;

		void start() override;
		void exit() override;




	private:
		

		void setup() override;

		void initGLFW() override;
		const char* initBackend() override;
		void initWindow() override;
		void initGLAD() override;
		void initImGUI(const char* backend_version = nullptr) override;
		void getBackendVersion() override;

		void enableVerticalSync(bool enable) override;
		void enableMouseCapture(bool enable) override;

		static void check_vk_result(VkResult err);
		static bool isExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension);
		static void setupVulkan(ImVector<const char*> instance_extensions);
		static void setupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
		static void cleanupVulkan();
		static void cleanupVulkanWindow(ImGui_ImplVulkanH_Window* wd);
		static void frameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
		static void framePresent(ImGui_ImplVulkanH_Window* wd);

		static void GLAPIENTRY vulkanDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

		
	};
}