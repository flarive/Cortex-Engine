#include "../../../include/renderers/vulkan/vulkan_phong_renderer.h"

#include "../../../include/singleton.h"

#include "../../../include/managers/filesystem_manager.h"
#include "../../../include/debug/opengl_debug.h"
#include "../../../include/debug/debug_frame.h"

#include "../../../include/lights/spot_light.h"
#include "../../../include/lights/point_light.h"
#include "../../../include/lights/directional_light.h"

#include "../../../include/managers/texture_manager.h"

#include "../../../include/misc/colors.h"


engine::VulkanPhongRenderer::VulkanPhongRenderer(GLFWwindow* window)
    : VulkanRenderer(window, RenderMethod::Phong)
{
    logger.trace("VulkanPhongRenderer constructor called");
}

void engine::VulkanPhongRenderer::setup(int width, int height, const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights)
{
    
}

void engine::VulkanPhongRenderer::loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update, std::function<void()> updateUI, const std::string& hash)
{
    
}

void engine::VulkanPhongRenderer::extractBloomBrightPass(float bloomThreshold)
{

}

void engine::VulkanPhongRenderer::blurBloomTexture(ubyte bloomBlurPasses)
{

}

void engine::VulkanPhongRenderer::initRenderStates(const SceneSettings& settings)
{

}

void engine::VulkanPhongRenderer::loadShaders()
{
    // Phong illumination model and lightning shader
    phongShader = std::make_unique<VulkanShader>("phong");
    phongShader->init("shaders/renderer/phong/phong.vert", "shaders/renderer/phong/phong.frag");
    phongShader->createUniformBuffer("PhongCameraUBO", 0, sizeof(PhongCameraUBO));
    phongShader->createUniformBuffer("PhongModelUBO", 1, sizeof(PhongModelUBO));
    phongShader->createUniformBuffer("PhongUBO", 2, sizeof(PhongUBO));

    static_assert(sizeof(PhongCameraUBO) % 16 == 0);
    static_assert(sizeof(PhongModelUBO) % 16 == 0);
    static_assert(sizeof(PhongUBO) % 16 == 0);

    // shared shaders
    VulkanRenderer::loadShaders();
}

void engine::VulkanPhongRenderer::initCommonTextures()
{

}

void engine::VulkanPhongRenderer::initFramebuffers(const SceneSettings& settings, int width, int height)
{

}

void engine::VulkanPhongRenderer::setupShaderBindings(const SceneSettings& settings)
{

}

void engine::VulkanPhongRenderer::setupScreenShaderBindings()
{

}

void engine::VulkanPhongRenderer::setupBackgroundShaderBindings()
{

}

void engine::VulkanPhongRenderer::initCaptureMatrices()
{

}

void engine::VulkanPhongRenderer::initCaptureResources(int vsize)
{

}

void engine::VulkanPhongRenderer::generateEnvironmentMap(const SceneSettings& settings, int vsize)
{

}

void engine::VulkanPhongRenderer::generateIrradianceMap()
{

}

void engine::VulkanPhongRenderer::generatePrefilterMap()
{

}

void engine::VulkanPhongRenderer::generateBRDFLUT(int vsize)
{

}

void engine::VulkanPhongRenderer::restoreScreenViewport()
{

}

void engine::VulkanPhongRenderer::beginFrame(const SceneSettings& settings, int width, int height)
{

}

void engine::VulkanPhongRenderer::renderDebug(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{

}

void engine::VulkanPhongRenderer::renderSkybox(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{

}

void engine::VulkanPhongRenderer::setupPBRPass(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{

}

void engine::VulkanPhongRenderer::renderScene(std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update)
{
    update(phongShader, phongShader);
}

void engine::VulkanPhongRenderer::renderShadowPass(int width, int height, const engine::SceneSettings& settings, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update)
{

}

void engine::VulkanPhongRenderer::resolveMSAA(int width, int height)
{

}

void engine::VulkanPhongRenderer::renderBloom(const engine::SceneSettings& settings)
{

}

void engine::VulkanPhongRenderer::renderFramebuffer(int width, int height, const engine::SceneSettings& settings)
{

}

void engine::VulkanPhongRenderer::renderUI(std::function<void()> updateUI)
{
    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::VulkanPhongRenderer::endFrame()
{

}

void engine::VulkanPhongRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount)
{

}

void engine::VulkanPhongRenderer::initBloomBuffers(int width, int height)
{

}

std::unique_ptr<engine::Shader>& engine::VulkanPhongRenderer::getShader()
{
    return phongShader;
}

void engine::VulkanPhongRenderer::clean()
{
    Renderer::clean();

    logger.info("Cleaning up Phong renderer resources");
}

engine::VulkanPhongRenderer::~VulkanPhongRenderer()
{
    logger.trace("VulkanPhongRenderer destructor called");
}
