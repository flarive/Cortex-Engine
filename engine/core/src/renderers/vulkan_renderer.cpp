#include "../../include/renderers/vulkan_renderer.h"


engine::VulkanRenderer::VulkanRenderer(GLFWwindow* window, RenderMethod method)
    : Renderer(window, method)
{
    logger.trace("VulkanRenderer base constructor called");
}

void engine::VulkanRenderer::loadShaders()
{
    int i = 0;
}

engine::VulkanRenderer::~VulkanRenderer()
{
    logger.trace("VulkanRenderer base destructor called");
}