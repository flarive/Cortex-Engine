#include "../../../include/renderers/opengl/opengl_phong_renderer.h"

#include "../../../include/singleton.h"

#include "../../../include/managers/filesystem_manager.h"
#include "../../../include/managers/texture_manager.h"

#include "../../../include/debug/opengl_debug.h"
#include "../../../include/debug/debug_frame.h"

#include "../../../include/lights/spot_light.h"
#include "../../../include/lights/point_light.h"
#include "../../../include/lights/directional_light.h"

#include "../../../include/misc/colors.h"




engine::OpenGLPhongRenderer::OpenGLPhongRenderer(GLFWwindow* window)
    : OpenGLRenderer(window, RenderMethod::Phong)
{
    logger.trace("PhongRenderer constructor called");
}

void engine::OpenGLPhongRenderer::setup(int width, int height, const std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights)
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
    setupBackgroundShaderBindings();

	initFramebuffers(settings, width, height);

    // solid/wireframe polygons
    RenderState::enableWireframe();
}

void engine::OpenGLPhongRenderer::loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update, std::function<void()> updateUI)
{
    //DebugFrame::ensureIsCalledOncePerFrame("PhongRenderer", "loop");
    
    m_camera = camera;

    const SceneSettings& settings = Singleton::getInstance()->sceneSettings();

    beginFrame(settings, width, height);

    FrameData frameData = buildFrameData(width, height);

    // --- 1) SETUP PHONG SHADER ---
    setupPhongPass(frameData, settings);

    // --- 2) RENDER SCENE ENTITIES HIERARCHY ---
    renderScene(update);

    resolveMSAA(width, height);

    // --- 3) UI ---
    renderUI(updateUI);

    endFrame();
}

void engine::OpenGLPhongRenderer::initRenderStates(const SceneSettings& settings)
{
    // avoid computing back faces not visible by camera
    enableFaceCulling(false);
    enableDepthTest(false);
    enableStencilTest(false);
}

void engine::OpenGLPhongRenderer::loadShaders()
{
    // blinn phong illumination model and lightning shader
    phongShader = std::make_unique<OpenGLShader>("phong");
    phongShader->init("shaders/renderer/phong/phong.vert", "shaders/renderer/phong/phong.frag");
    phongShader->createUniformBuffer("PhongCameraUBO", 0, sizeof(PhongCameraUBO));
    phongShader->createUniformBuffer("PhongModelUBO", 1, sizeof(PhongModelUBO));
    phongShader->createUniformBuffer("PhongUBO", 2, sizeof(PhongUBO));

    static_assert(sizeof(PhongCameraUBO) % 16 == 0);
    static_assert(sizeof(PhongModelUBO) % 16 == 0);
    static_assert(sizeof(PhongUBO) % 16 == 0);


    // shared shaders
    Renderer::loadShaders();
}

void engine::OpenGLPhongRenderer::initCommonTextures()
{
    // hardcoded texture UV mapper just for test purpose and to keep Phong renderer very simple
    sampleTextureID = TextureManager::loadTexture("textures/uv_mapper.jpg", TextureFlag_InvertY);
}

void engine::OpenGLPhongRenderer::initFramebuffers(const SceneSettings& settings, int width, int height)
{
	// no framebuffer needed for Phong renderer, rendering directly to screen
}

void engine::OpenGLPhongRenderer::setupShaderBindings(const SceneSettings& settings)
{
    phongShader->use();
    phongShader->setInt("texture_diffuse", 0);
}

void engine::OpenGLPhongRenderer::setupScreenShaderBindings()
{
    
}

void engine::OpenGLPhongRenderer::setupBackgroundShaderBindings()
{

}

void engine::OpenGLPhongRenderer::beginFrame(const SceneSettings& settings, int width, int height)
{
    // solid color background
    Color backgroundColor = Colors::Black;
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a); // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderState::disableWireframe();

    updateEditorPropertySettings(width, height);
}

void engine::OpenGLPhongRenderer::setupPhongPass(const engine::FrameData& frameData, const engine::SceneSettings& settings)
{
    phongShader->use();
    /*phongShader->setMat4("projection", frameData.projection);
    phongShader->setMat4("view", frameData.view);
    phongShader->setVec3("viewPos", frameData.viewPos);
    phongShader->setVec3("lightPos", glm::vec3(0.0f, 10.0f, 10.0f));
    phongShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));*/

    /*phongShader->setMat4("projection", frameData.projection);
    phongShader->setMat4("view", frameData.view);*/

    PhongCameraUBO phongCameraUBO{ frameData.view, frameData.projection };
    phongShader->updateUniformBuffer("PhongCameraUBO", &phongCameraUBO, sizeof(PhongCameraUBO));

    PhongUBO phongUBO{};
    phongUBO.lightPos = glm::vec4(0.0f, 10.0f, 10.0f, 1.0f);
    phongUBO.viewPos = glm::vec4(frameData.viewPos, 1.0f);
    phongUBO.lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    phongUBO.objectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    

    
    
    
    // hardcoded sample texture
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, sampleTextureID);
    phongUBO.hasDiffuseTexture = 1;
    //phongShader->setBool("hasDiffuseTexture", true);
    //phongShader->setInt("texture_diffuse", 0);
    
    phongShader->updateUniformBuffer("PhongUBO", &phongUBO, sizeof(PhongUBO));
    
    //phongShader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
}

void engine::OpenGLPhongRenderer::renderScene(std::function<void(std::unique_ptr<Shader>&, std::unique_ptr<Shader>&)> update)
{
    update(phongShader, phongShader);
}

void engine::OpenGLPhongRenderer::resolveMSAA(int width, int height)
{
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
    //glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void engine::OpenGLPhongRenderer::renderUI(std::function<void()> updateUI)
{
    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::OpenGLPhongRenderer::endFrame()
{
}

void engine::OpenGLPhongRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount)
{
    m_pointLightCount = pointLightCount;
    m_dirLightCount = dirLightCount;
    m_spotLightCount = spotLightCount;
    m_areaLightCount = areaLightCount;

    // not really needed
}

std::unique_ptr<engine::Shader>& engine::OpenGLPhongRenderer::getShader()
{
    return phongShader;
}

void engine::OpenGLPhongRenderer::clean()
{
    Renderer::clean();

    logger.info("Cleaning up Phong renderer resources");
    
    // delete shaders
    phongShader->clean();
}

engine::OpenGLPhongRenderer::~OpenGLPhongRenderer()
{
    logger.trace("PhongRenderer destructor called");
}
