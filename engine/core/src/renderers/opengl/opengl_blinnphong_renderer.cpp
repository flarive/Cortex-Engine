#include "../../../include/renderers/opengl/opengl_blinnphong_renderer.h"

#include "../../../include/singleton.h"

#include "../../../include/managers/filesystem_manager.h"
#include "../../../include/debug/opengl_debug.h"
#include "../../../include/debug/debug_frame.h"

#include "../../../include/lights/spot_light.h"
#include "../../../include/lights/point_light.h"
#include "../../../include/lights/directional_light.h"


#include "../../../include/managers/texture_manager.h"

#include "../../../include/misc/colors.h"

engine::OpenGLBlinnPhongRenderer::OpenGLBlinnPhongRenderer(GLFWwindow* window)
    : OpenGLRenderer(window, RenderMethod::BlinnPhong)
{
    logger.trace("BlinnPhongRenderer constructor called");
}

void engine::OpenGLBlinnPhongRenderer::setup(int width, int height, const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights)
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
    setupBackgroundShaderBindings();
    
    initBackground();

    initFramebuffers(settings, width, height);

    if (settings.showDebugGrid)
        initDebugPlaneGrid();
}

void engine::OpenGLBlinnPhongRenderer::setSkybox(const std::vector<std::string>& faces)
{
    m_skybox = std::make_shared<Skybox>();
    m_skybox->setup(faces);
}

void engine::OpenGLBlinnPhongRenderer::loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update, std::function<void()> updateUI, const std::string& hash)
{
#ifdef DEBUG
    DebugFrame::ensureIsCalledOncePerFrame("OpenGLBlinnPhongRenderer", "loop", hash);
#endif
    m_camera = camera;
    
    const SceneSettings& settings = Singleton::getInstance()->sceneSettings();

    beginFrame(settings, width, height);

    FrameData frameData = buildFrameData(width, height);

    // debug grid
    renderDebug(frameData, settings);

    // --- 1) SKYBOX FIRST (depth writes OFF, depth test ON) ---
    renderSkybox(frameData, settings);

    // --- 2) SETUP BLINNPHONG SHADERS ---
    setupBlinnPhongPass(frameData, settings);

    // --- 3) RENDER SCENE ENTITIES HIERARCHY ---
    renderScene(update);

    // --- 4) RENDER SHADOW MAP PASS ---
    renderShadowPass(width, height, settings, update);

    // --- 5) RESOLVE MSAA TO HDR RESOLVE FBO ---
    resolveMSAA(width, height);

    // --- 6) RENDER FRAMEBUFFER TO SCREEN (TONE MAP / POSTPROCESS) ---
    renderFramebuffer(settings);

    // --- 7) RENDER UI ---
    renderUI(updateUI);

    endFrame();
}

void engine::OpenGLBlinnPhongRenderer::initRenderStates(const SceneSettings& settings)
{
    // configure global opengl state
    // -----------------------------
    // enable depth testing
    enableDepthTest(true);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    RenderState::enableCubeMapSeamless();
    // enable objects outlining
    enableStencilTest(true);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // avoid computing back faces not visible by camera
    enableFaceCulling(settings.enableFaceCulling);
}

void engine::OpenGLBlinnPhongRenderer::loadShaders()
{
    // blinn phong illumination model and lightning shader
    blinnPhongShader = std::make_unique<OpenGLShader>("blinnphong");
    blinnPhongShader->init("shaders/renderer/blinnphong/blinn-phong.vert", "shaders/renderer/blinnphong/blinn-phong.frag");
    
    blinnPhongShaderTessellation = std::make_unique<OpenGLShader>("blinnphongTessellation");
    blinnPhongShaderTessellation->init("shaders/renderer/blinnphong/blinn-phong.vert", "shaders/renderer/shared/height.tcs", "shaders/renderer/shared/height.tes", nullptr, "shaders/renderer/blinnphong/blinn-phong.frag");
    

    // skybox reflection shader
    skyboxReflectionShader = std::make_unique<OpenGLShader>("cubemap");
    skyboxReflectionShader->init("shaders/renderer/blinnphong/blinnphong_cubemap.vert", "shaders/renderer/blinnphong/blinnphong_cubemap.frag");

	// gradient background shader
    backgroundShader = std::make_unique<OpenGLShader>("background");
    backgroundShader->init("shaders/renderer/blinnphong/blinnphong_background.vert", "shaders/renderer/blinnphong/blinnphong_background.frag");

    // shared shaders
    OpenGLRenderer::loadShaders();
}

void engine::OpenGLBlinnPhongRenderer::initCommonTextures()
{
    // Load LTC textures
    LTC1Map = TextureManager::loadMTexture();
    LTC2Map = TextureManager::loadLUTTexture();
}

void engine::OpenGLBlinnPhongRenderer::initFramebuffers(const SceneSettings& settings, int width, int height)
{
    // Depth map framebuffer configuration (for shadow map)
    // -----------------------------------
    initDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);

    // color framebuffer configuration
    // -------------------------
    initFramebuffer(RenderMethod::BlinnPhong, width, height, settings.frameBufferAntiAliasingSamplesQuality);
}

void engine::OpenGLBlinnPhongRenderer::setupShaderBindings(const SceneSettings& settings)
{
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    blinnPhongShader->use();
    blinnPhongShader->setFloat("material.shadowIntensity", settings.shadowIntensity);
    blinnPhongShader->setInt("material.shadowCalculationMethod", settings.shadowCalculationMethod);
    blinnPhongShader->setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    blinnPhongShader->setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
    blinnPhongShader->setInt("LTC1", U_LTC1); // Tell the shader to use texture unit 20 for LTC1
    blinnPhongShader->setInt("LTC2", U_LTC2); // Tell the shader to use texture unit 21 for LTC2
    // shadowMapCube must be linked before first update call (avoid openGL error 1282)
    blinnPhongShader->setInt("texture_shadowMapCube", U_SHADOW_MAP_CUBE);
    blinnPhongShader->setBool("hasShadowMapCube", m_lights[0]->isShadowCaster());

    if (supportTessellation())
    {
        blinnPhongShaderTessellation->use();
        blinnPhongShaderTessellation->setFloat("material.shadowIntensity", settings.shadowIntensity);
        blinnPhongShaderTessellation->setInt("material.shadowCalculationMethod", settings.shadowCalculationMethod);
        blinnPhongShaderTessellation->setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
        blinnPhongShaderTessellation->setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
        blinnPhongShaderTessellation->setInt("LTC1", U_LTC1); // Tell the shader to use texture unit 20 for LTC1
        blinnPhongShaderTessellation->setInt("LTC2", U_LTC2); // Tell the shader to use texture unit 21 for LTC2
        // shadowMapCube must be linked before first update call (avoid openGL error 1282)
        blinnPhongShaderTessellation->setInt("texture_shadowMapCube", U_SHADOW_MAP_CUBE);
        blinnPhongShaderTessellation->setBool("hasShadowMapCube", m_lights[0]->isShadowCaster());
    }
}

void engine::OpenGLBlinnPhongRenderer::setupScreenShaderBindings()
{
    screenShader->use();
    screenShader->setInt("screenTexture", 0);
}

void engine::OpenGLBlinnPhongRenderer::setupBackgroundShaderBindings()
{
    
}

void engine::OpenGLBlinnPhongRenderer::initBackground()
{
    float vertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void engine::OpenGLBlinnPhongRenderer::renderBackground(const SceneSettings& settings)
{
    // make sure we clear the framebuffer's content
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
    
    if (settings.backgroundGradientColors.enabled)
    {
        // gradient color background
        backgroundShader->use();
        backgroundShader->setVec4("topcolor", settings.backgroundGradientColors.topColor);
        backgroundShader->setVec4("bottomcolor", settings.backgroundGradientColors.bottomColor);
        backgroundShader->setFloat("ySplit", settings.backgroundGradientColors.ySplit);

        glBindVertexArray(backgroundVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Only clear depth and stencil
    }
    else
    {
        // solid color background
        Color backgroundColor = settings.backgroundColor;
        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a); // background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
}

void engine::OpenGLBlinnPhongRenderer::beginFrame(const SceneSettings& settings, int width, int height)
{
    // bind to color framebuffer and draw scene as we normally would to color texture 
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    RenderState::enableDepthTest(); // enable depth testing (is disabled for rendering screen-space quad)

    renderBackground(settings); // Render your gradient or custom background

    updateEditorPropertySettings(width, height);

    // Wireframe toggle
    if (settings.drawAsWireframe)
        RenderState::enableWireframe();
    else
        RenderState::disableWireframe();
}

void engine::OpenGLBlinnPhongRenderer::renderDebug(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    if (settings.showDebugGrid)
        renderDebugPlaneGrid(frameData.projection, frameData.view);
}

void engine::OpenGLBlinnPhongRenderer::renderSkybox(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    if (m_skybox)
        m_skybox->draw(frameData.projection, frameData.view);  // this handles view matrix stripping translation inside
}

void engine::OpenGLBlinnPhongRenderer::setupBlinnPhongPass(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    blinnPhongShader->use();
    blinnPhongShader->setMat4("projection", frameData.projection);
    blinnPhongShader->setMat4("view", frameData.view);
    blinnPhongShader->setVec3("viewPos", frameData.viewPos);
    blinnPhongShader->setFloat("material.shadowIntensity", settings.shadowIntensity);
    blinnPhongShader->setInt("material.shadowCalculationMethod", static_cast<int>(settings.shadowCalculationMethod));
    blinnPhongShader->setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    blinnPhongShader->setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);

    if (supportTessellation())
    {
        blinnPhongShaderTessellation->use();
        blinnPhongShaderTessellation->setMat4("projection", frameData.projection);
        blinnPhongShaderTessellation->setMat4("view", frameData.view);
        blinnPhongShaderTessellation->setVec3("viewPos", frameData.viewPos);
        blinnPhongShaderTessellation->setFloat("material.shadowIntensity", settings.shadowIntensity);
        blinnPhongShaderTessellation->setInt("material.shadowCalculationMethod", static_cast<int>(settings.shadowCalculationMethod));
        blinnPhongShaderTessellation->setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
        blinnPhongShaderTessellation->setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
    }

    // Bind LTC
    glActiveTexture(GL_TEXTURE0 + U_LTC1);
    glBindTexture(GL_TEXTURE_2D, LTC1Map);
    glActiveTexture(GL_TEXTURE0 + U_LTC2);
    glBindTexture(GL_TEXTURE_2D, LTC2Map);
}

void engine::OpenGLBlinnPhongRenderer::renderScene(std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update)
{
    update(blinnPhongShader, blinnPhongShaderTessellation);
    //update(outlineColorShader);
}

void engine::OpenGLBlinnPhongRenderer::renderShadowPass(int width, int height, const engine::SceneSettings& settings, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update)
{
    // compute light shadows using a depth map framebuffer
    computeDepthMapFramebuffer(width, height, settings.enableShadows, (GLsizei)settings.shadowMapsTextureSize, blinnPhongShader, blinnPhongShaderTessellation, update);
}

void engine::OpenGLBlinnPhongRenderer::resolveMSAA(int width, int height)
{
    // Resolve MSAA to screen or another texture FBO
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void engine::OpenGLBlinnPhongRenderer::renderFramebuffer(const engine::SceneSettings& settings)
{
    // render to framebuffer
    computeColorFramebuffer(settings);
}

void engine::OpenGLBlinnPhongRenderer::renderUI(std::function<void()> updateUI)
{
    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::OpenGLBlinnPhongRenderer::endFrame()
{

}

void engine::OpenGLBlinnPhongRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount)
{
    m_pointLightCount = pointLightCount;
    m_dirLightCount = dirLightCount;
    m_spotLightCount = spotLightCount;
    m_areaLightCount = areaLightCount;

    auto setCounts = [this](std::unique_ptr<engine::Shader>& sh) {
            sh->use();
            sh->setInt("pointLightsCount", m_pointLightCount);
            sh->setInt("dirLightsCount", m_dirLightCount);
            sh->setInt("spotLightsCount", m_spotLightCount);
            sh->setInt("areaLightsCount", m_areaLightCount);
    };

    setCounts(blinnPhongShader);
    setCounts(blinnPhongShaderTessellation);
}

std::unique_ptr<engine::Shader>& engine::OpenGLBlinnPhongRenderer::getShader()
{
    return blinnPhongShader;
}

void engine::OpenGLBlinnPhongRenderer::clean()
{
    Renderer::clean();

    logger.info("Cleaning up BlinnPhong renderer resources");
    
    // delete shaders
    blinnPhongShader->clean();
    blinnPhongShaderTessellation->clean();
    skyboxReflectionShader->clean();
    backgroundShader->clean();

	m_skybox->clean();
}



engine::OpenGLBlinnPhongRenderer::~OpenGLBlinnPhongRenderer()
{
    logger.trace("BlinnPhongRenderer destructor called");
}