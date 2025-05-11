#include "../../include/renderers/blinnphong_renderer.h"


engine::BlinnPhongRenderer::BlinnPhongRenderer(GLFWwindow* window, const engine::SceneSettings& settings)
    : Renderer(window, settings)
{
}


void engine::BlinnPhongRenderer::setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<engine::Light>>& lights)
{
    m_lights = lights;
    
    // configure global opengl state
    // -----------------------------
    enableDepthTest(true);
    enableFaceCulling(true);
    enableAntiAliasing(true);
    if (m_settings.applyGammaCorrection) enableGammaCorrection(true);


    loadShaders();


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    blinnPhongShader.use();
    blinnPhongShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    blinnPhongShader.setFloat("material.shininess", 32.0f);

    // shader configuration
    // --------------------
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // Depth map framebuffer configuration (for shadow map)
    // -----------------------------------
    initDepthMapFramebuffer();

    // color framebuffer configuration
    // -------------------------
    initColorFramebuffer(width, height);

    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_LINE
}


void engine::BlinnPhongRenderer::loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&)> update, std::function<void()> updateUI)
{
    // bind to color framebuffer and draw scene as we normally would to color texture 
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

    // make sure we clear the framebuffer's content
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update user stuffs
    update(blinnPhongShader);

    // compute light shadows using a depth map framebuffer
    if (m_lights.size() > 0)
        computeDepthMapFramebuffer(blinnPhongShader, width, height, update, m_lights[0]);

    // render to framebuffer
    computeColorFramebuffer();

    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::BlinnPhongRenderer::loadShaders()
{
    // blinn phong illumination model and lightning shader
    blinnPhongShader.init("blinnphong", "shaders/blinn-phong.vertex", "shaders/blinn-phong.frag");

    // skybox reflection shader
    skyboxShader.init("cubemap", "shaders/cubemap.vertex", "shaders/cubemap.frag");

    Renderer::loadShaders();
}

void engine::BlinnPhongRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount)
{
    m_pointLightCount = pointLightCount;
    m_dirLightCount = dirLightCount;
    m_spotLightCount = spotLightCount;


    //make the same for blinnphong shader !
    blinnPhongShader.use();
    blinnPhongShader.setInt("pointLightsCount", m_pointLightCount);
    blinnPhongShader.setInt("dirLightsCount", m_dirLightCount);
    blinnPhongShader.setInt("spotLightsCount", m_spotLightCount);
}