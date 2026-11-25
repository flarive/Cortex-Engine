#include "../../../include/renderers/vulkan/vulkan_pbr_renderer.h"

#include "../../../include/singleton.h"

#include "../../../include/managers/filesystem_manager.h"
#include "../../../include/debug/opengl_debug.h"
#include "../../../include/debug/debug_frame.h"

#include "../../../include/lights/spot_light.h"
#include "../../../include/lights/point_light.h"
#include "../../../include/lights/directional_light.h"

#include "../../../include/managers/texture_manager.h"

#include "../../../include/misc/colors.h"


engine::VulkanPbrRenderer::VulkanPbrRenderer(GLFWwindow* window)
    : VulkanRenderer(window, RenderMethod::PBR)
{
    logger.trace("VulkanPbrRenderer constructor called");
}

void engine::VulkanPbrRenderer::setup(int width, int height, const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights)
{
    m_lights = lights;
    m_camera = camera;

    auto* singleton = engine::Singleton::getInstance();
    const SceneSettings& settings = singleton->sceneSettings();


    initRenderStates(settings);

    // build and compile shaders
    // -------------------------
    loadShaders();

    initCommonTextures();

    // shader configuration
    // --------------------
    setupShaderBindings(settings);
    setupScreenShaderBindings();


    initFramebuffers(settings, width, height);


    int vsize{ 512 };

    if (settings.showDebugGrid)
        initDebugPlaneGrid();

    // init IBL framebuffer
    // --------------------
    initCaptureResources(vsize);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    initCaptureMatrices();


    // IBL generation
    // --------------
    generateEnvironmentMap(settings, vsize);
    generateIrradianceMap();
    generatePrefilterMap();
    generateBRDFLUT(vsize);

    // PBR background
    setupBackgroundShaderBindings();

    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    restoreScreenViewport();
}

void engine::VulkanPbrRenderer::loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update, std::function<void()> updateUI, const std::string& hash)
{
    m_camera = camera;

    const SceneSettings& settings = Singleton::getInstance()->sceneSettings();

    beginFrame(settings, width, height);

    FrameData frameData = buildFrameData(width, height);

    // debug grid
    renderDebug(frameData, settings);

    // --- 1) SKYBOX FIRST (depth writes OFF, depth test ON) ---
    renderSkybox(frameData, settings);

    // --- 2) SETUP PBR SHADERS & IBL ---
    setupPBRPass(frameData, settings);

    // --- 3) RENDER SCENE ENTITIES HIERARCHY ---
    renderScene(update);

    // --- 4) RENDER SHADOW MAP PASS ---
    renderShadowPass(width, height, settings, update);

    // --- 5) RESOLVE MSAA TO HDR RESOLVE FBO ---
    resolveMSAA(width, height);

    // --- 6) RENDER BLOOM POST FX
    renderBloom(settings);


    // --- 6) TONE MAP / POSTPROCESS TO SCREEN ---
    renderFramebuffer(width, height, settings);

    // --- 7) UI ---
    renderUI(updateUI);

    endFrame();
}

void engine::VulkanPbrRenderer::extractBloomBrightPass(float bloomThreshold)
{
    
}

void engine::VulkanPbrRenderer::blurBloomTexture(ubyte bloomBlurPasses)
{
    
}

void engine::VulkanPbrRenderer::initRenderStates(const SceneSettings& settings)
{
    
}

void engine::VulkanPbrRenderer::loadShaders()
{
    // PBR shaders
    pbrShader = std::make_unique<VulkanShader>("pbr");
    pbrShader->init("shaders/renderer/pbr/pbr.vert", "shaders/renderer/pbr/pbr.frag");

    //pbrShaderTessellation = std::make_unique<OpenGLShader>("pbrTessellation");
    //pbrShaderTessellation->init("shaders/renderer/pbr/pbr.vert", "shaders/renderer/shared/height.tcs", "shaders/renderer/shared/height.tes", nullptr, "shaders/renderer/pbr/pbr.frag");

    //equirectangularToCubemapShader = std::make_unique<OpenGLShader>("equirectangularToCubemapShader");
    //equirectangularToCubemapShader->init("shaders/renderer/pbr/pbr_cubemap.vert", "shaders/renderer/pbr/equirectangular_to_cubemap.frag");

    //irradianceShader = std::make_unique<OpenGLShader>("irradianceShader");
    //irradianceShader->init("shaders/renderer/pbr/pbr_cubemap.vert", "shaders/renderer/pbr/irradiance_convolution.frag");

    //prefilterShader = std::make_unique<OpenGLShader>("prefilterShader");
    //prefilterShader->init("shaders/renderer/pbr/pbr_cubemap.vert", "shaders/renderer/pbr/prefilter.frag");

    //brdfShader = std::make_unique<OpenGLShader>("brdfShader");
    //brdfShader->init("shaders/renderer/pbr/brdf.vert", "shaders/renderer/pbr/brdf.frag");

    //// HDR skybox shader
    //backgroundShader = std::make_unique<OpenGLShader>("background");
    //backgroundShader->init("shaders/renderer/pbr/pbr_background.vert", "shaders/renderer/pbr/pbr_background.frag");

    // shared shaders
    VulkanRenderer::loadShaders();
}

void engine::VulkanPbrRenderer::initCommonTextures()
{

}

void engine::VulkanPbrRenderer::initFramebuffers(const SceneSettings& settings, int width, int height)
{
    
}

void engine::VulkanPbrRenderer::setupShaderBindings(const SceneSettings& settings)
{
    
}

void engine::VulkanPbrRenderer::setupScreenShaderBindings()
{
    
}

void engine::VulkanPbrRenderer::setupBackgroundShaderBindings()
{
    
}

void engine::VulkanPbrRenderer::initCaptureMatrices()
{

}

void engine::VulkanPbrRenderer::initCaptureResources(int vsize)
{
    
}

void engine::VulkanPbrRenderer::generateEnvironmentMap(const SceneSettings& settings, int vsize)
{
    
}

void engine::VulkanPbrRenderer::generateIrradianceMap()
{
    
}

void engine::VulkanPbrRenderer::generatePrefilterMap()
{
    
}

void engine::VulkanPbrRenderer::generateBRDFLUT(int vsize)
{
    
}

void engine::VulkanPbrRenderer::restoreScreenViewport()
{

}

void engine::VulkanPbrRenderer::beginFrame(const SceneSettings& settings, int width, int height)
{
    
}

void engine::VulkanPbrRenderer::renderDebug(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    
}

void engine::VulkanPbrRenderer::renderSkybox(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    
}

void engine::VulkanPbrRenderer::setupPBRPass(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    
}

void engine::VulkanPbrRenderer::renderScene(std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update)
{
    // In your engine, `update(pbrShader, pbrShaderTessellation)` currently draws everything.
    // You want to split it into:
    //   updateOpaque(pbrShader, pbrShaderTessellation);
    //   updateTransparent(pbrShader, pbrShaderTessellation);
    //
    // For now, assume update() draws opaque first, then transparent (sorted).
    update(pbrShader, pbrShaderTessellation);
}

void engine::VulkanPbrRenderer::renderShadowPass(int width, int height, const engine::SceneSettings& settings, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update)
{

}

void engine::VulkanPbrRenderer::resolveMSAA(int width, int height)
{
    
}

void engine::VulkanPbrRenderer::renderBloom(const engine::SceneSettings& settings)
{
    
}

void engine::VulkanPbrRenderer::renderFramebuffer(int width, int height, const engine::SceneSettings& settings)
{
    
}

void engine::VulkanPbrRenderer::renderUI(std::function<void()> updateUI)
{
    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::VulkanPbrRenderer::endFrame()
{
    
}

void engine::VulkanPbrRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount)
{

}

void engine::VulkanPbrRenderer::initBloomBuffers(int width, int height)
{
    
}

std::unique_ptr<engine::Shader>& engine::VulkanPbrRenderer::getShader()
{
    return pbrShader;
}

void engine::VulkanPbrRenderer::clean()
{
    Renderer::clean();

    logger.info("Cleaning up PBR renderer resources");
}

engine::VulkanPbrRenderer::~VulkanPbrRenderer()
{
    logger.trace("VulkanPbrRenderer destructor called");
}