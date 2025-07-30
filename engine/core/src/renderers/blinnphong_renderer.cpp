#include "../../include/renderers/blinnphong_renderer.h"

#include "../../include/tools/file_system.h"


#include "../../include/lights/spot_light.h"
#include "../../include/lights/point_light.h"
#include "../../include/lights/directional_light.h"




engine::BlinnPhongRenderer::BlinnPhongRenderer(GLFWwindow* window, const engine::SceneSettings& settings)
    : Renderer(window, settings)
{
}


void engine::BlinnPhongRenderer::setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights)
{
    m_lights = lights;
    m_camera = camera;
    
    // configure global opengl state
    // -----------------------------
    enableDepthTest(true);
    enableFaceCulling(true);
    if (m_settings.applyGammaCorrection) enableGammaCorrection(true);


    loadShaders();


    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    blinnPhongShader.use();
    blinnPhongShader.setFloat("material.shadowIntensity", m_settings.shadowIntensity);

    // shader configuration
    // --------------------
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // Depth map framebuffer configuration (for shadow map)
    // -----------------------------------
    
    if (m_lights.size() > 0)
    {
        auto firstLight = m_lights[0];
        if (std::dynamic_pointer_cast<PointLight>(firstLight))
            initDepthMapFramebuffer2();
        else
            initDepthMapFramebuffer();
    }

    // color framebuffer configuration
    // -------------------------
    initColorFramebufferMSAA(width, height);

    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_LINE
}

void engine::BlinnPhongRenderer::setSkybox(std::shared_ptr<Skybox> skybox)
{
    m_skybox = skybox;
}

void engine::BlinnPhongRenderer::loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&)> update, std::function<void()> updateUI)
{
    // bind to color framebuffer and draw scene as we normally would to color texture 
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

    // make sure we clear the framebuffer's content
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();


    // draw skybox FIRST
    if (m_skybox)
        m_skybox->draw(projection, view);  // this handles view matrix stripping translation inside




    blinnPhongShader.use();
    blinnPhongShader.setMat4("projection", projection);
    blinnPhongShader.setMat4("view", view);
    blinnPhongShader.setVec3("viewPos", camera->position);


    // update user stuffs
    update(blinnPhongShader);

    // compute light shadows using a depth map framebuffer
    if (m_lights.size() > 0)
    {
        auto firstLight = m_lights[0];
        if (std::dynamic_pointer_cast<PointLight>(firstLight))
            computeDepthMapFramebuffer2(blinnPhongShader, width, height, update, firstLight);
        else
            computeDepthMapFramebuffer(blinnPhongShader, width, height, update, firstLight);
    }

    // render to framebuffer
    computeColorFramebuffer();

    // Resolve MSAA to screen or another texture FBO
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
    glBlitFramebuffer(0, 0, width, height,
        0, 0, width, height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);

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

engine::Shader& engine::BlinnPhongRenderer::getShader()
{
    return blinnPhongShader;
}