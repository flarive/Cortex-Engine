#pragma once

#include "app.h"

namespace engine
{
	class VulkanApp : public App
	{
	public:
		VulkanApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, AppSettings _settings);
		~VulkanApp() override;

		void start() override;
		void exit() override;




	private:
		void setup() override;

		void initGLFW() override;
		const char* initBackend() override;
		void initImGUI(const char* backend_version = nullptr) override;

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