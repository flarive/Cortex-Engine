#include "../../../include/renderers/opengl/opengl_pbr_renderer.h"

#include "../../../include/singleton.h"

#include "../../../include/managers/filesystem_manager.h"
#include "../../../include/debug/opengl_debug.h"
#include "../../../include/debug/debug_frame.h"

#include "../../../include/lights/spot_light.h"
#include "../../../include/lights/point_light.h"
#include "../../../include/lights/directional_light.h"

#include "../../../include/managers/texture_manager.h"

#include "../../../include/misc/colors.h"


engine::OpenGLPbrRenderer::OpenGLPbrRenderer(GLFWwindow* window)
    : OpenGLRenderer(window, RenderMethod::PBR)
{
    logger.trace("PbrRenderer constructor called");
}

/// <summary>
/// Query once per program startup and cache these caps, not every bind.
/// GLint maxFragUnits = 16;
/// glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);
/// </summary>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="camera"></param>
/// <param name="lights"></param>
void engine::OpenGLPbrRenderer::setup(int width, int height, const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights)
{
    m_lights = lights;
    m_camera = camera;

    const SceneSettings& settings = engine::Singleton::getInstance()->sceneSettings();


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

void engine::OpenGLPbrRenderer::loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update, std::function<void()> updateUI)
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

void engine::OpenGLPbrRenderer::extractBloomBrightPass(float bloomThreshold)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_bloomFBO);

    RenderState::disableDepthTest();

    bloomBrightExtractionShader->use();
	bloomBrightExtractionShader->setFloat("bloomThreshold", bloomThreshold); // or 2.0f, depending on your scene's brightness

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

    renderQuad();

    RenderState::enableDepthTest();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void engine::OpenGLPbrRenderer::blurBloomTexture(ubyte bloomBlurPasses)
{
    bool horizontal = true;
    bool firstIteration = true;

    for (int i = 0; i < bloomBlurPasses; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingPongFBO[horizontal]);

        RenderState::disableDepthTest();

        bloomBlurShader->use();
        bloomBlurShader->setBool("horizontal", horizontal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, firstIteration ? m_bloomBrightTexture : m_pingPongColorBuffer[!horizontal]);

        renderQuad();

        RenderState::enableDepthTest();

        horizontal = !horizontal;
        firstIteration = false;
    }

    // final blurred image
    m_finalBloomTexture = m_pingPongColorBuffer[!horizontal];
}

void engine::OpenGLPbrRenderer::initRenderStates(const SceneSettings& settings)
{
    // configure global opengl state
    // -----------------------------
    // enable depth testing
    enableDepthTest(true);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    RenderState::enableCubeMapSeamless();
    // enable objects outlining
    enableStencilTest(true);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // avoid computing back faces not visible by camera
    enableFaceCulling(settings.enableFaceCulling);
}

void engine::OpenGLPbrRenderer::loadShaders()
{
    // PBR shaders
    pbrShader = std::make_unique<OpenGLShader>("pbr");
    pbrShader->init("shaders/renderer/pbr/pbr.vert", "shaders/renderer/pbr/pbr.frag");
    
    pbrShaderTessellation = std::make_unique<OpenGLShader>("pbrTessellation");
    pbrShaderTessellation->init("shaders/renderer/pbr/pbr.vert", "shaders/renderer/shared/height.tcs", "shaders/renderer/shared/height.tes", nullptr, "shaders/renderer/pbr/pbr.frag");

    equirectangularToCubemapShader = std::make_unique<OpenGLShader>("equirectangularToCubemapShader");
    equirectangularToCubemapShader->init("shaders/renderer/pbr/pbr_cubemap.vert", "shaders/renderer/pbr/equirectangular_to_cubemap.frag");
    
    irradianceShader = std::make_unique<OpenGLShader>("irradianceShader");
    irradianceShader->init("shaders/renderer/pbr/pbr_cubemap.vert", "shaders/renderer/pbr/irradiance_convolution.frag");
    
    prefilterShader = std::make_unique<OpenGLShader>("prefilterShader");
    prefilterShader->init("shaders/renderer/pbr/pbr_cubemap.vert", "shaders/renderer/pbr/prefilter.frag");
    
    brdfShader = std::make_unique<OpenGLShader>("brdfShader");
    brdfShader->init("shaders/renderer/pbr/brdf.vert", "shaders/renderer/pbr/brdf.frag");

    // HDR skybox shader
    backgroundShader = std::make_unique<OpenGLShader>("background");
    backgroundShader->init("shaders/renderer/pbr/pbr_background.vert", "shaders/renderer/pbr/pbr_background.frag");

    // shared shaders
    Renderer::loadShaders();
}

void engine::OpenGLPbrRenderer::initCommonTextures()
{
	// Load LTC textures
	LTC1Map = TextureManager::loadMTexture();
	LTC2Map = TextureManager::loadLUTTexture();
}

void engine::OpenGLPbrRenderer::initFramebuffers(const SceneSettings& settings, int width, int height)
{
    // Depth map framebuffer configuration (for shadow map)
    // -----------------------------------
    initDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);


    // color framebuffer configuration
    // -------------------------
    initFramebuffer(RenderMethod::PBR, width, height, settings.frameBufferAntiAliasingSamplesQuality); // HDR and AA


	// bloom framebuffer configuration
    // -------------------------
    if (settings.enableBloom)
        initBloomBuffers(width, height);
}

void engine::OpenGLPbrRenderer::setupShaderBindings(const SceneSettings& settings)
{
    pbrShader->use();
    pbrShader->setInt("material.texture_irradiance", U_IRR); // Should be texture unit, not texture ID
    pbrShader->setInt("material.texture_prefilter", U_PREF); // Should be texture unit, not texture ID
    pbrShader->setInt("material.texture_brdfLUT", U_BRDF); // Should be texture unit, not texture ID
    pbrShader->setInt("LTC1", U_LTC1); // Should be texture unit, not texture ID
    pbrShader->setInt("LTC2", U_LTC2); // Should be texture unit, not texture ID
    pbrShader->setFloat("material.shadowIntensity", settings.shadowIntensity);
    pbrShader->setInt("material.shadowCalculationMethod", settings.shadowCalculationMethod);
    pbrShader->setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    pbrShader->setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
    pbrShader->setFloat("material.iblDiffuseIntensity", settings.iblDiffuseIntensity); // [0.0, 2.0]
    pbrShader->setFloat("material.iblSpecularIntensity", settings.iblSpecularIntensity); // [0.0, 5.0]
    // shadowMapCube must be linked before first update call (avoid openGL error 1282)
    pbrShader->setInt("texture_shadowMapCube", U_SHADOW_MAP_CUBE);
    pbrShader->setBool("hasShadowMapCube", m_lights[0]->isShadowCaster());

    if (supportTessellation())
    {
        pbrShaderTessellation->use();
        pbrShaderTessellation->setInt("material.texture_irradiance", U_IRR); // Should be texture unit, not texture ID
        pbrShaderTessellation->setInt("material.texture_prefilter", U_PREF); // Should be texture unit, not texture ID
        pbrShaderTessellation->setInt("material.texture_brdfLUT", U_BRDF); // Should be texture unit, not texture ID
        pbrShaderTessellation->setInt("LTC1", U_LTC1); // Should be texture unit, not texture ID
        pbrShaderTessellation->setInt("LTC2", U_LTC2); // Should be texture unit, not texture ID
        pbrShaderTessellation->setFloat("material.shadowIntensity", settings.shadowIntensity);
        pbrShaderTessellation->setInt("material.shadowCalculationMethod", settings.shadowCalculationMethod);
        pbrShaderTessellation->setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
        pbrShaderTessellation->setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
        pbrShaderTessellation->setFloat("material.iblDiffuseIntensity", settings.iblDiffuseIntensity); // [0.0, 2.0]
        pbrShaderTessellation->setFloat("material.iblSpecularIntensity", settings.iblSpecularIntensity); // [0.0, 5.0]
        // shadowMapCube must be linked before first update call (avoid openGL error 1282)
        pbrShaderTessellation->setInt("texture_shadowMapCube", U_SHADOW_MAP_CUBE);
        pbrShaderTessellation->setBool("hasShadowMapCube", m_lights[0]->isShadowCaster());
    }
}

void engine::OpenGLPbrRenderer::setupScreenShaderBindings()
{
    screenShader->use();
    screenShader->setInt("screenTexture", 0); // Should be texture unit, not texture ID
}

void engine::OpenGLPbrRenderer::setupBackgroundShaderBindings()
{
    backgroundShader->use();
    backgroundShader->setInt("environmentMap", U_BG_ENV); // Should be texture unit, not texture ID
}


void engine::OpenGLPbrRenderer::initCaptureMatrices()
{
    m_captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    m_captureViews =
    {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +X (right)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // -X (left)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)), // +Y (top)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)), // -Y (bottom)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +Z (front)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)) // -Z (back)
    };
}

void engine::OpenGLPbrRenderer::initCaptureResources(int vsize)
{
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, vsize, vsize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // Unbind both framebuffer and renderbuffer to restore state
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void engine::OpenGLPbrRenderer::generateEnvironmentMap(const SceneSettings& settings, int vsize)
{
    // pbr: load the HDR environment map
    // ---------------------------------
    unsigned int hdrTexture = !settings.HDRSkyboxFilePath.empty() ? engine::TextureManager::loadHDRImage(FileSystemManager::getPath(settings.HDRSkyboxFilePath)) : 0;

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, vsize, vsize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    equirectangularToCubemapShader->use();
    equirectangularToCubemapShader->setInt("equirectangularMap", 0);
    equirectangularToCubemapShader->setMat4("projection", m_captureProjection);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, vsize, vsize); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader->setMat4("view", m_captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void engine::OpenGLPbrRenderer::generateIrradianceMap()
{
    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    glActiveTexture(GL_TEXTURE0); // Reset to default texture unit
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind the renderbuffer
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Unbind the cubemap texture




    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    irradianceShader->use();
    irradianceShader->setInt("environmentMap", 0);
    irradianceShader->setMat4("projection", m_captureProjection);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader->setMat4("view", m_captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }

    glActiveTexture(GL_TEXTURE0); // Reset active texture unit to default
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Unbind the cubemap texture
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind renderbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
    glUseProgram(0); // Unbind shader program
}

void engine::OpenGLPbrRenderer::generatePrefilterMap()
{
    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    prefilterShader->use();
    prefilterShader->setInt("environmentMap", 0);
    prefilterShader->setMat4("projection", m_captureProjection);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels{ 5 };
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // Resize framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader->setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader->setMat4("view", m_captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }


    glActiveTexture(GL_TEXTURE0); // Reset active texture unit to default
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind renderbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
    glUseProgram(0); // Unbind shader program
}

void engine::OpenGLPbrRenderer::generateBRDFLUT(int vsize)
{
    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    glGenTextures(1, &brdfLUTTexture);
    // Pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, vsize, vsize, 0, GL_RG, GL_FLOAT, 0);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the 2D texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, vsize, vsize);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, vsize, vsize);
    brdfShader->use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glActiveTexture(GL_TEXTURE0); // Reset active texture unit to default
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
    glUseProgram(0); // Unbind shader program
}

void engine::OpenGLPbrRenderer::restoreScreenViewport()
{
    int scrWidth{}, scrHeight{};
    glfwGetFramebufferSize(m_window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
}


void engine::OpenGLPbrRenderer::beginFrame(const SceneSettings& settings, int width, int height)
{
    // Bind HDR/MSAA color framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    RenderState::enableDepthTest();
    RenderState::enableStencilTest();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    updateEditorPropertySettings(width, height);

    // Wireframe toggle
    if (settings.drawAsWireframe)
        RenderState::enableWireframe();
    else
        RenderState::disableWireframe();
}


void engine::OpenGLPbrRenderer::renderDebug(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    if (settings.showDebugGrid)
        renderDebugPlaneGrid(frameData.projection, frameData.view);
}

void engine::OpenGLPbrRenderer::renderSkybox(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    RenderState::disableDepthMask();
    backgroundShader->use();
    backgroundShader->setMat4("view", frameData.view);
    backgroundShader->setMat4("projection", frameData.projection);
    backgroundShader->setFloat("blurStrength", settings.HDRSkyboxBlurStrength);

    glActiveTexture(GL_TEXTURE0 + U_BG_ENV);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    if (!settings.HDRSkyboxHide)
        renderCube();

    RenderState::enableDepthMask(); // restore depth writes
}

void engine::OpenGLPbrRenderer::setupPBRPass(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    pbrShader->use();
    pbrShader->setMat4("projection", frameData.projection);
    pbrShader->setMat4("view", frameData.view);
    pbrShader->setVec3("viewPos", frameData.viewPos);
    pbrShader->setFloat("material.shadowIntensity", settings.shadowIntensity);
    pbrShader->setInt("material.shadowCalculationMethod", (int)settings.shadowCalculationMethod);
    pbrShader->setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    pbrShader->setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
    pbrShader->setFloat("material.iblDiffuseIntensity", settings.iblDiffuseIntensity);
    pbrShader->setFloat("material.iblSpecularIntensity", settings.iblSpecularIntensity);

    if (supportTessellation())
    {
        pbrShaderTessellation->use();
        pbrShaderTessellation->setMat4("projection", frameData.projection);
        pbrShaderTessellation->setMat4("view", frameData.view);
        pbrShaderTessellation->setVec3("viewPos", frameData.viewPos);
        pbrShaderTessellation->setFloat("material.shadowIntensity", settings.shadowIntensity);
        pbrShaderTessellation->setInt("material.shadowCalculationMethod", (int)settings.shadowCalculationMethod);
        pbrShaderTessellation->setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
        pbrShaderTessellation->setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
        pbrShaderTessellation->setFloat("material.iblDiffuseIntensity", settings.iblDiffuseIntensity);
        pbrShaderTessellation->setFloat("material.iblSpecularIntensity", settings.iblSpecularIntensity);
    }

    // Bind IBL + LTC
    glActiveTexture(GL_TEXTURE0 + U_IRR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glActiveTexture(GL_TEXTURE0 + U_PREF);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE0 + U_BRDF);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    glActiveTexture(GL_TEXTURE0 + U_LTC1);
    glBindTexture(GL_TEXTURE_2D, LTC1Map);
    glActiveTexture(GL_TEXTURE0 + U_LTC2);
    glBindTexture(GL_TEXTURE_2D, LTC2Map);
}

void engine::OpenGLPbrRenderer::renderScene(std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update)
{
    // In your engine, `update(pbrShader, pbrShaderTessellation)` currently draws everything.
    // You want to split it into:
    //   updateOpaque(pbrShader, pbrShaderTessellation);
    //   updateTransparent(pbrShader, pbrShaderTessellation);
    //
    // For now, assume update() draws opaque first, then transparent (sorted).
    update(pbrShader, pbrShaderTessellation);
}

void engine::OpenGLPbrRenderer::renderShadowPass(int width, int height, const engine::SceneSettings& settings, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update)
{
    computeDepthMapFramebuffer(width, height,
        settings.enableShadows,
        (GLsizei)settings.shadowMapsTextureSize,
        pbrShader,
        pbrShaderTessellation,
        update);
}

void engine::OpenGLPbrRenderer::resolveMSAA(int width, int height)
{
    // Resolve MSAA to screen or another texture FBO
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void engine::OpenGLPbrRenderer::renderBloom(const engine::SceneSettings& settings)
{
    if (settings.enableBloom)
    {
        extractBloomBrightPass(settings.bloomThreshold);
        blurBloomTexture(settings.bloomBlurPasses);
    }
}

void engine::OpenGLPbrRenderer::renderFramebuffer(int width, int height, const engine::SceneSettings& settings)
{
    // render to framebuffer
    computeHDRColorFramebuffer(width, height, settings);
}

void engine::OpenGLPbrRenderer::renderUI(std::function<void()> updateUI)
{
    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::OpenGLPbrRenderer::endFrame()
{
    // test glDepthMask(GL_FALSE) leakage
    GLboolean depthMask;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
    assert(depthMask == GL_TRUE);
}

void engine::OpenGLPbrRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount)
{
    m_pointLightCount = pointLightCount;
    m_dirLightCount = dirLightCount;
    m_spotLightCount = spotLightCount;
    m_areaLightCount = areaLightCount;

    auto setCounts = [this](std::unique_ptr<Shader>& sh) {
        sh->use();
        sh->setInt("pointLightsCount", m_pointLightCount);
        sh->setInt("dirLightsCount", m_dirLightCount);
        sh->setInt("spotLightsCount", m_spotLightCount);
        sh->setInt("areaLightsCount", m_areaLightCount);
        };

    setCounts(pbrShader);
    setCounts(pbrShaderTessellation);
}

void engine::OpenGLPbrRenderer::initBloomBuffers(int width, int height)
{
    //----------------------------------------------------------------------
    // Bright-pass framebuffer
    //----------------------------------------------------------------------

    glGenFramebuffers(1, &m_bloomFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_bloomFBO);

    glGenTextures(1, &m_bloomBrightTexture);
    glBindTexture(GL_TEXTURE_2D, m_bloomBrightTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bloomBrightTexture,
        0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        logger.error("Bloom framebuffer is not complete");
    }

    //----------------------------------------------------------------------
    // Ping-pong framebuffers for Gaussian blur
    //----------------------------------------------------------------------

    glGenFramebuffers(2, m_pingPongFBO);
    glGenTextures(2, m_pingPongColorBuffer);

    for (int i = 0; i < 2; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingPongFBO[i]);

        glBindTexture(GL_TEXTURE_2D, m_pingPongColorBuffer[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingPongColorBuffer[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            logger.error("Bloom ping-pong framebuffer {} is not complete", i);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    bloomBrightExtractionShader->use();
    bloomBrightExtractionShader->setInt("screenTexture", 0);

    bloomBlurShader->use();
    bloomBlurShader->setInt("image", 0);
}

std::unique_ptr<engine::Shader>& engine::OpenGLPbrRenderer::getShader()
{
    return pbrShader;
}

void engine::OpenGLPbrRenderer::clean()
{
	Renderer::clean();

    logger.info("Cleaning up PBR renderer resources");
    
    // delete shaders
    pbrShader->clean();
    backgroundShader->clean();

    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
    glDeleteTextures(1, &envCubemap);
    glDeleteTextures(1, &irradianceMap);
    glDeleteTextures(1, &prefilterMap);
    glDeleteTextures(1, &brdfLUTTexture);

    glDeleteFramebuffers(1, &m_bloomFBO);
    glDeleteFramebuffers(2, m_pingPongFBO);
    glDeleteTextures(1, &m_bloomBrightTexture);
    glDeleteTextures(2, m_pingPongColorBuffer);
}

engine::OpenGLPbrRenderer::~OpenGLPbrRenderer()
{
    logger.trace("PbrRenderer destructor called");
}