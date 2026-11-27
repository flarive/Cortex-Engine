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

bool engine::VulkanRenderer::bind(std::unique_ptr<Shader>& shader, const std::vector<std::shared_ptr<Texture>>& textures, int baseUnit, bool resetHasTextures) const
{
    return false;
}

void engine::VulkanRenderer::unbind(const std::vector<std::shared_ptr<Texture>>& textures, int baseUnit) const
{
}

void engine::VulkanRenderer::renderModel(const std::shared_ptr<Model> model, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform)
{

}

void engine::VulkanRenderer::renderMesh(const std::shared_ptr<Mesh> mesh, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix)
{

}

void engine::VulkanRenderer::renderPrimitive(const std::shared_ptr<Primitive> primitive, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform)
{

}

void engine::VulkanRenderer::renderParticleSystem(const std::shared_ptr<ParticleSystem> primitive, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform, const glm::mat4& projection, const glm::mat4& view)
{

}

void engine::VulkanRenderer::renderTerrain(const std::shared_ptr<Terrain> primitive, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform)
{

}


void engine::VulkanRenderer::uploadPrimitive(const std::shared_ptr<Primitive> primitive)
{

}

engine::VulkanRenderer::~VulkanRenderer()
{
    logger.trace("VulkanRenderer base destructor called");
}