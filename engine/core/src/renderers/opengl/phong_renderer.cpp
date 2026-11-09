#include "../../../include/renderers/opengl/phong_renderer.h"

#include "../../../include/singleton.h"

#include "../../../include/managers/filesystem_manager.h"
#include "../../../include/debug/opengl_debug.h"
#include "../../../include/debug/debug_frame.h"

#include "../../../include/lights/spot_light.h"
#include "../../../include/lights/point_light.h"
#include "../../../include/lights/directional_light.h"

#include "../../../include/misc/colors.h"


engine::OpenGLPhongRenderer::OpenGLPhongRenderer(GLFWwindow* window)
    : Renderer(window, RenderMethod::Phong)
{
    logger.trace("PhongRenderer constructor called");
}

void engine::OpenGLPhongRenderer::setup(int width, int height, std::shared_ptr<Camera>& camera, const std::vector<std::shared_ptr<Light>>& lights)
{
    m_lights = lights;
    m_camera = camera;


    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    initRenderStates(settings);
    
    // build and compile shaders
    // -------------------------
    loadShaders();



    // shader configuration
    // --------------------
    setupShaderBindings(settings);
    setupScreenShaderBindings();
    setupBackgroundShaderBindings();

	initFramebuffers(settings, width, height);

    // solid/wireframe polygons
    RenderState::enableWireframe();
}

void engine::OpenGLPhongRenderer::loop(int width, int height, std::shared_ptr<Camera>& camera, std::function<void(std::unique_ptr<ShaderBase>&, std::unique_ptr<ShaderBase>&)> update, std::function<void()> updateUI)
{
    m_camera = camera;

    //DebugFrame::ensureIsCalledOncePerFrame("PhongRenderer", "loop");

    glm::mat4 projection = camera->getProjectionMatrix(width * 1.0f / height * 1.0f);
    glm::mat4 view = camera->getViewMatrix();

    // solid color background
    Color backgroundColor = Colors::Black;
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a); // background color
    glClear(GL_COLOR_BUFFER_BIT);

    updateEditorPropertySettings(width, height);

    phongShader->use();
    phongShader->setMat4("projection", projection);
    phongShader->setMat4("view", view);
    phongShader->setVec3("viewPos", camera->position);
    phongShader->setVec3("lightPos", glm::vec3(0.0f, 10.0f, 10.0f));
    phongShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    phongShader->setVec3("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
    

    // update user stuffs
    update(phongShader, phongShader);

    // Resolve MSAA to screen or another texture FBO
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
    ////glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
    //glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::OpenGLPhongRenderer::initRenderStates(const SceneSettings& settings)
{
    // avoid computing back faces not visible by camera
    enableFaceCulling(false);
}

void engine::OpenGLPhongRenderer::loadShaders()
{
    // blinn phong illumination model and lightning shader
    phongShader = std::make_unique<OpenGLShader>("blinnphong");
    phongShader->init("shaders/renderer/phong/phong.vert", "shaders/renderer/phong/phong.frag");

    // shared shaders
    Renderer::loadShaders();
}

void engine::OpenGLPhongRenderer::initFramebuffers(const SceneSettings& settings, int width, int height)
{
	// no framebuffer needed for Phong renderer, rendering directly to screen
}


void engine::OpenGLPhongRenderer::setupShaderBindings(const SceneSettings& settings)
{

}

void engine::OpenGLPhongRenderer::setupScreenShaderBindings()
{
    
}

void engine::OpenGLPhongRenderer::setupBackgroundShaderBindings()
{

}

void engine::OpenGLPhongRenderer::beginFrame(const SceneSettings& settings, int width, int height)
{

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

std::unique_ptr<engine::ShaderBase>& engine::OpenGLPhongRenderer::getShader()
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
