#include "../../include/renderers/blinnphong_renderer.h"

#include "../../include/singleton.h"

#include "../../include/tools/file_system.h"
#include "../../include/debug/opengl_debug.h"

#include "../../include/lights/spot_light.h"
#include "../../include/lights/point_light.h"
#include "../../include/lights/directional_light.h"

#include "../../include/misc/colors.h"


engine::BlinnPhongRenderer::BlinnPhongRenderer(GLFWwindow* window)
    : Renderer(window)
{
}


void engine::BlinnPhongRenderer::setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights)
{
    m_lights = lights; // copy ?????????
    m_camera = camera; // copy ?????????

    // configure global opengl state
    // -----------------------------
    // enable depth testing
    enableDepthTest(true);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // enable objects outlining
    enableStencilTest(true);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


    auto* singleton = engine::Singleton::getInstance();
	assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    // avoid computing back faces not visible by camera
    enableFaceCulling(settings.enableFaceCulling);
    
    // automatic color correction
    enableGammaCorrection(settings.enableGammaCorrection);
   

    // build and compile shaders
    // -------------------------
    loadShaders();

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    blinnPhongShader.use();
    blinnPhongShader.setFloat("material.shadowIntensity", settings.shadowIntensity);


    LTC1Map = Texture::loadMTexture();
    LTC2Map = Texture::loadLUTTexture();

    //glActiveTexture(GL_TEXTURE0 + 20);
    //glBindTexture(GL_TEXTURE_2D, LTC1Map);
    //glActiveTexture(GL_TEXTURE0 + 21);
    //glBindTexture(GL_TEXTURE_2D, LTC2Map);


    //blinnPhongShader.setInt("LTC1", 20); // Tell the shader to use texture unit 20 for LTC1
    //blinnPhongShader.setInt("LTC2", 21); // Tell the shader to use texture unit 21 for LTC2


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
            initDepthMapFramebuffer2((GLsizei)settings.shadowMapsTextureSize);
        else
            initDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);
    }

    // color framebuffer configuration
    // -------------------------
    initColorFramebufferMSAA(width, height);

    // solid/wireframe polygons
    glPolygonMode(GL_FRONT_AND_BACK, settings.drawAsWireframe ? GL_LINE : GL_FILL);
}

void engine::BlinnPhongRenderer::setSkybox(const std::vector<std::string>& faces)
{
    m_skybox = std::make_shared<Skybox>();
    m_skybox->setup(faces);
}

void engine::BlinnPhongRenderer::loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&)> update, std::function<void()> updateUI)
{
    // bind to color framebuffer and draw scene as we normally would to color texture 
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

    // make sure we clear the framebuffer's content
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    updateSettings();



    glm::mat4 projection = glm::perspective(glm::radians(camera->zoom), (float)width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();


    // draw skybox first
    if (m_skybox)
        m_skybox->draw(projection, view);  // this handles view matrix stripping translation inside




    blinnPhongShader.use();
    blinnPhongShader.setMat4("projection", projection);
    blinnPhongShader.setMat4("view", view);
    blinnPhongShader.setVec3("viewPos", camera->position);
    blinnPhongShader.setFloat("material.shadowIntensity", settings.shadowIntensity);


    // should be moved in init !!!!!!!!!!!!!!!!!!!!!!
    // bind pre-computed area light LTC data
    glActiveTexture(GL_TEXTURE0 + 20);
    glBindTexture(GL_TEXTURE_2D, LTC1Map);
    glActiveTexture(GL_TEXTURE0 + 21);
    glBindTexture(GL_TEXTURE_2D, LTC2Map);
    blinnPhongShader.setInt("LTC1", 20); // Tell the shader to use texture unit 20 for LTC1
    blinnPhongShader.setInt("LTC2", 21); // Tell the shader to use texture unit 21 for LTC2


    // update user stuffs
    update(blinnPhongShader);
    //update(outlineColorShader);


    // compute light shadows using a depth map framebuffer
    if (m_lights.size() > 0)
    {
        auto firstLight = m_lights[0];
        if (std::dynamic_pointer_cast<PointLight>(firstLight))
            computeDepthMapFramebuffer2(blinnPhongShader, width, height, settings.enableShadows, (GLsizei)settings.shadowMapsTextureSize, update, firstLight);
        else
            computeDepthMapFramebuffer(blinnPhongShader, width, height, settings.enableShadows, (GLsizei)settings.shadowMapsTextureSize, update, firstLight);
    }

    // render to framebuffer
    computeColorFramebuffer();

    // Resolve MSAA to screen or another texture FBO
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::BlinnPhongRenderer::loadShaders()
{
    // blinn phong illumination model and lightning shader
    blinnPhongShader.init("blinnphong", "shaders/blinn-phong.vert", "shaders/blinn-phong.frag");

    // skybox reflection shader
    skyboxShader.init("cubemap", "shaders/cubemap.vert", "shaders/cubemap.frag");

    // shared shaders
    Renderer::loadShaders();
}

void engine::BlinnPhongRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount)
{
    m_pointLightCount = pointLightCount;
    m_dirLightCount = dirLightCount;
    m_spotLightCount = spotLightCount;
    m_areaLightCount = areaLightCount;

    blinnPhongShader.use();
    blinnPhongShader.setInt("pointLightsCount", m_pointLightCount);
    blinnPhongShader.setInt("dirLightsCount", m_dirLightCount);
    blinnPhongShader.setInt("spotLightsCount", m_spotLightCount);
    blinnPhongShader.setInt("areaLightsCount", m_areaLightCount);
}

engine::Shader& engine::BlinnPhongRenderer::getShader()
{
    return blinnPhongShader;
}

void engine::BlinnPhongRenderer::clean()
{
    Renderer::clean();
    
    // delete shaders
    blinnPhongShader.clean();
    skyboxShader.clean();

	m_skybox->clean();
}