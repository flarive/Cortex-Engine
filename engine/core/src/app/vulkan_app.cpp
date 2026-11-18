#include "../../include/app/vulkan_app.h"



engine::VulkanApp::VulkanApp(const std::string& _title, unsigned int _width, unsigned int _height, bool _fullscreen, const AppSettings& _settings)
    : App(_title, _width, _height, _fullscreen, _settings)
{
    logger.trace("VulkanApp {} constructor called", _title);

    setup();
}

void engine::VulkanApp::start()
{

}

void engine::VulkanApp::setup()
{
    initGLFW();

    initBackend();

    initWindow();

    setApp(window);

    // boilerplate stuff (ie. basic window setup, initialize OpenGL) occurs in abstract class
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    enableVerticalSync(true);

    enableMouseCapture(true);

    initGLAD();

    initImGUI();

    getBackendVersion();

    registerWindowInputCallbacks();
}

void engine::VulkanApp::initGLFW()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        logger.error("GLFW init failed");
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x MSAA
}

const char* engine::VulkanApp::initBackend()
{
    // Vulkan initialization
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Because handling resized windows takes special care that we'll look into later, disable it for now with another window hint call
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if (!glfwVulkanSupported())
    {
        logger.error("GLFW: Vulkan Not Supported\n");
    }

    return nullptr;
}

void engine::VulkanApp::getBackendVersion()
{
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(VulkanGlobals::g_PhysicalDevice, &props);

    // store in singleton
    AppSettings& appSettings = engine::Singleton::getInstance()->appSettings();
    appSettings.backend = BackendType::Vulkan;

    std::string vendor = "Unknown";

    switch (props.vendorID)
    {
    case 0x10DE: vendor = "NVIDIA"; break;
    case 0x1002: vendor = "AMD"; break;
    case 0x8086: vendor = "Intel"; break;
    }

    appSettings.gpuVendor = vendor;
    appSettings.gpuRenderer = props.deviceName;
    appSettings.gpuVersion = fmt::format("{}.{}.{}", VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion), VK_VERSION_PATCH(props.apiVersion));
}

void engine::VulkanApp::initWindow()
{
    GLFWmonitor* myMonitor = glfwGetPrimaryMonitor(); // The primary monitor

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE); // Request debug context

    const GLFWvidmode* mode = glfwGetVideoMode(myMonitor);

    // Create window with graphics context
    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), "Cortex engine", NULL, nullptr);
    if (window == NULL)
    {
        logger.error("Failed to create GLFW window");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    //if (fullscreen)
    //    toggleFullscreen([this]() {});

    //glfwMakeContextCurrent(window);
}

void engine::VulkanApp::initGLAD()
{

}

void engine::VulkanApp::initImGUI(const char* backend_version)
{
    App::initImGUI(backend_version);

    //#define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
    static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
#endif

    ImVector<const char*> extensions;
    uint32_t extensions_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    for (uint32_t i = 0; i < extensions_count; i++)
        extensions.push_back(glfw_extensions[i]);
    setupVulkan(extensions);

    // Create Window Surface
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(VulkanGlobals::g_Instance, window, VulkanGlobals::g_Allocator, &surface);
    check_vk_result(err);

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    ImGui_ImplVulkanH_Window* wd = &VulkanGlobals::g_MainWindowData;
    setupVulkanWindow(wd, surface, w, h);


    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.ApiVersion = VulkanGlobals::g_ApiVersion;
    init_info.Instance = VulkanGlobals::g_Instance;
    init_info.PhysicalDevice = VulkanGlobals::g_PhysicalDevice;
    init_info.Device = VulkanGlobals::g_Device;
    init_info.QueueFamily = VulkanGlobals::g_QueueFamily;
    init_info.Queue = VulkanGlobals::g_Queue;
    init_info.PipelineCache = VulkanGlobals::g_PipelineCache;
    init_info.DescriptorPool = VulkanGlobals::g_DescriptorPool;
    init_info.MinImageCount = VulkanGlobals::g_MinImageCount;
    init_info.ImageCount = wd->ImageCount;
    init_info.Allocator = VulkanGlobals::g_Allocator;
    init_info.PipelineInfoMain.RenderPass = wd->RenderPass;
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info);

}

void engine::VulkanApp::enableVerticalSync(bool enable)
{
}

void engine::VulkanApp::enableMouseCapture(bool enable)
{
}

void GLAPIENTRY engine::VulkanApp::vulkanDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
}




void engine::VulkanApp::check_vk_result(VkResult err)
{
    if (err == VK_SUCCESS)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}


#ifdef APP_USE_VULKAN_DEBUG_REPORT
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
    fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
    return VK_FALSE;
}
#endif // APP_USE_VULKAN_DEBUG_REPORT



bool engine::VulkanApp::isExtensionAvailable(const ImVector<VkExtensionProperties>& properties, const char* extension)
{
    for (const VkExtensionProperties& p : properties)
        if (strcmp(p.extensionName, extension) == 0)
            return true;
    return false;
}

void engine::VulkanApp::setupVulkan(ImVector<const char*> instance_extensions)
{
    VkResult err;
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
    volkInitialize();
#endif

    uint32_t supportedVersion = VK_API_VERSION_1_0;
    vkEnumerateInstanceVersion(&supportedVersion);

    VulkanGlobals::g_ApiVersion = supportedVersion;

    // Create Vulkan Instance
    {
        VkInstanceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // Enumerate available extensions
        uint32_t properties_count;
        ImVector<VkExtensionProperties> properties;
        vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
        properties.resize(properties_count);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.Data);
        check_vk_result(err);

        // Enable required extensions
        if (isExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
            instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        if (isExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
        {
            instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        }
#endif

        // Enabling validation layers
#ifdef APP_USE_VULKAN_DEBUG_REPORT
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = layers;
        instance_extensions.push_back("VK_EXT_debug_report");
#endif

        // Create Vulkan Instance
        create_info.enabledExtensionCount = (uint32_t)instance_extensions.Size;
        create_info.ppEnabledExtensionNames = instance_extensions.Data;
        err = vkCreateInstance(&create_info, VulkanGlobals::g_Allocator, &VulkanGlobals::g_Instance);
        check_vk_result(err);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
        volkLoadInstance(VulkanGlobals::g_Instance);
#endif

        // Setup the debug report callback
#ifdef APP_USE_VULKAN_DEBUG_REPORT
        auto f_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(VulkanGlobals::g_Instance, "vkCreateDebugReportCallbackEXT");
        IM_ASSERT(f_vkCreateDebugReportCallbackEXT != nullptr);
        VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
        debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        debug_report_ci.pfnCallback = debug_report;
        debug_report_ci.pUserData = nullptr;
        err = f_vkCreateDebugReportCallbackEXT(VulkanGlobals::g_Instance, &debug_report_ci, VulkanGlobals::g_Allocator, &VulkanGlobals::g_DebugReport);
        check_vk_result(err);
#endif
    }

    // Select Physical Device (GPU)
    VulkanGlobals::g_PhysicalDevice = ImGui_ImplVulkanH_SelectPhysicalDevice(VulkanGlobals::g_Instance);
    IM_ASSERT(VulkanGlobals::g_PhysicalDevice != VK_NULL_HANDLE);

    // Select graphics queue family
    VulkanGlobals::g_QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(VulkanGlobals::g_PhysicalDevice);
    IM_ASSERT(VulkanGlobals::g_QueueFamily != (uint32_t)-1);

    // Create Logical Device (with 1 queue)
    {
        ImVector<const char*> device_extensions;
        device_extensions.push_back("VK_KHR_swapchain");

        // Enumerate physical device extension
        uint32_t properties_count;
        ImVector<VkExtensionProperties> properties;
        vkEnumerateDeviceExtensionProperties(VulkanGlobals::g_PhysicalDevice, nullptr, &properties_count, nullptr);
        properties.resize(properties_count);
        vkEnumerateDeviceExtensionProperties(VulkanGlobals::g_PhysicalDevice, nullptr, &properties_count, properties.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
            device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

        const float queue_priority[] = { 1.0f };
        VkDeviceQueueCreateInfo queue_info[1] = {};
        queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info[0].queueFamilyIndex = VulkanGlobals::g_QueueFamily;
        queue_info[0].queueCount = 1;
        queue_info[0].pQueuePriorities = queue_priority;
        VkDeviceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
        create_info.pQueueCreateInfos = queue_info;
        create_info.enabledExtensionCount = (uint32_t)device_extensions.Size;
        create_info.ppEnabledExtensionNames = device_extensions.Data;
        err = vkCreateDevice(g_PhysicalDevice, &create_info, VulkanGlobals::g_Allocator, &VulkanGlobals::g_Device);
        check_vk_result(err);
        vkGetDeviceQueue(VulkanGlobals::g_Device, VulkanGlobals::g_QueueFamily, 0, &VulkanGlobals::g_Queue);
    }

    // Create Descriptor Pool
    // If you wish to load e.g. additional textures you may need to alter pools sizes and maxSets.
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE },
            { VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE },
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 0;
        for (VkDescriptorPoolSize& pool_size : pool_sizes)
            pool_info.maxSets += pool_size.descriptorCount;
        pool_info.poolSizeCount = (uint32_t)IM_COUNTOF(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        err = vkCreateDescriptorPool(VulkanGlobals::g_Device, &pool_info, VulkanGlobals::g_Allocator, &VulkanGlobals::g_DescriptorPool);
        check_vk_result(err);
    }
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
void engine::VulkanApp::setupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
{
    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(VulkanGlobals::g_PhysicalDevice, VulkanGlobals::g_QueueFamily, surface, &res);
    if (res != VK_TRUE)
    {
        fprintf(stderr, "Error no WSI support on physical device 0\n");
        //exit(-1);
    }

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    wd->Surface = surface;
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(VulkanGlobals::g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_COUNTOF(requestSurfaceImageFormat), requestSurfaceColorSpace);

    // Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(VulkanGlobals::g_PhysicalDevice, wd->Surface, &present_modes[0], IM_COUNTOF(present_modes));
    //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

    // Create SwapChain, RenderPass, Framebuffer, etc.
    IM_ASSERT(VulkanGlobals::g_MinImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(VulkanGlobals::g_Instance, VulkanGlobals::g_PhysicalDevice, VulkanGlobals::g_Device, wd, VulkanGlobals::g_QueueFamily, VulkanGlobals::g_Allocator, width, height, VulkanGlobals::g_MinImageCount, 0);
}

void engine::VulkanApp::cleanupVulkan()
{
    vkDestroyDescriptorPool(VulkanGlobals::g_Device, VulkanGlobals::g_DescriptorPool, VulkanGlobals::g_Allocator);

#ifdef APP_USE_VULKAN_DEBUG_REPORT
    // Remove the debug report callback
    auto f_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(VulkanGlobals::g_Instance, "vkDestroyDebugReportCallbackEXT");
    f_vkDestroyDebugReportCallbackEXT(VulkanGlobals::g_Instance, VulkanGlobals::g_DebugReport, VulkanGlobals::g_Allocator);
#endif // APP_USE_VULKAN_DEBUG_REPORT

    vkDestroyDevice(VulkanGlobals::g_Device, VulkanGlobals::g_Allocator);
    vkDestroyInstance(VulkanGlobals::g_Instance, VulkanGlobals::g_Allocator);
}

void engine::VulkanApp::cleanupVulkanWindow(ImGui_ImplVulkanH_Window* wd)
{
    ImGui_ImplVulkanH_DestroyWindow(VulkanGlobals::g_Instance, VulkanGlobals::g_Device, wd, VulkanGlobals::g_Allocator);
    vkDestroySurfaceKHR(VulkanGlobals::g_Instance, wd->Surface, VulkanGlobals::g_Allocator);
}

void engine::VulkanApp::frameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
{
    VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    VkResult err = vkAcquireNextImageKHR(VulkanGlobals::g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
        VulkanGlobals::g_SwapChainRebuild = true;
    if (err == VK_ERROR_OUT_OF_DATE_KHR)
        return;
    if (err != VK_SUBOPTIMAL_KHR)
        check_vk_result(err);

    ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
    {
        err = vkWaitForFences(VulkanGlobals::g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
        check_vk_result(err);

        err = vkResetFences(VulkanGlobals::g_Device, 1, &fd->Fence);
        check_vk_result(err);
    }
    {
        err = vkResetCommandPool(VulkanGlobals::g_Device, fd->CommandPool, 0);
        check_vk_result(err);
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
        check_vk_result(err);
    }
    {
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = wd->RenderPass;
        info.framebuffer = fd->Framebuffer;
        info.renderArea.extent.width = wd->Width;
        info.renderArea.extent.height = wd->Height;
        info.clearValueCount = 1;
        info.pClearValues = &wd->ClearValue;
        vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // Record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

    // Submit command buffer
    vkCmdEndRenderPass(fd->CommandBuffer);
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &image_acquired_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &fd->CommandBuffer;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &render_complete_semaphore;

        err = vkEndCommandBuffer(fd->CommandBuffer);
        check_vk_result(err);
        err = vkQueueSubmit(VulkanGlobals::g_Queue, 1, &info, fd->Fence);
        check_vk_result(err);
    }
}

void engine::VulkanApp::framePresent(ImGui_ImplVulkanH_Window* wd)
{
    if (VulkanGlobals::g_SwapChainRebuild)
        return;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &wd->Swapchain;
    info.pImageIndices = &wd->FrameIndex;
    VkResult err = vkQueuePresentKHR(VulkanGlobals::g_Queue, &info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
        VulkanGlobals::g_SwapChainRebuild = true;
    if (err == VK_ERROR_OUT_OF_DATE_KHR)
        return;
    if (err != VK_SUBOPTIMAL_KHR)
        check_vk_result(err);
    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
}

void engine::VulkanApp::exit()
{
    App::exit();

    // Cleanup
    VkResult err = vkDeviceWaitIdle(VulkanGlobals::g_Device);
    check_vk_result(err);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    cleanupVulkanWindow(&VulkanGlobals::g_MainWindowData);
    cleanupVulkan();

    glfwDestroyWindow(window);
    glfwTerminate();


    glfwDestroyWindow(window);

    glfwTerminate();
}

engine::VulkanApp::~VulkanApp()
{
    logger.trace("VulkanApp {} destructor called", m_title);
}
