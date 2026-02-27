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
    
    // build and compile shaders
    // -------------------------
    loadShaders();

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    blinnPhongShader.use();
    blinnPhongShader.setFloat("material.shadowIntensity", settings.shadowIntensity);
    blinnPhongShader.setInt("material.shadowCalculationMethod", settings.shadowCalculationMethod);
    blinnPhongShader.setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    blinnPhongShader.setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
    

    LTC1Map = Texture::loadMTexture();
    LTC2Map = Texture::loadLUTTexture();

    //glActiveTexture(GL_TEXTURE0 + 20);
    //glBindTexture(GL_TEXTURE_2D, LTC1Map);
    //glActiveTexture(GL_TEXTURE0 + 21);
    //glBindTexture(GL_TEXTURE_2D, LTC2Map);


    //blinnPhongShader.setInt("LTC1", 20); // Tell the shader to use texture unit 20 for LTC1
    //blinnPhongShader.setInt("LTC2", 21); // Tell the shader to use texture unit 21 for LTC2


    blinnPhongShader.setInt("LTC1", U_LTC1); // Tell the shader to use texture unit 20 for LTC1
    blinnPhongShader.setInt("LTC2", U_LTC2); // Tell the shader to use texture unit 21 for LTC2

    // shader configuration
    // --------------------
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // Depth map framebuffer configuration (for shadow map)
    // -----------------------------------
    initDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);

    initBackground();

    if (settings.showDebugGrid)
        initDebugPlaneGrid();

    // color framebuffer configuration
    // -------------------------
    initColorFramebufferMSAA(width, height); // SDR and AA
    //initColorFramebuffer(width, height); // SDR

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
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    glm::mat4 projection = camera->getProjectionMatrix(width, height, 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();

    
    // bind to color framebuffer and draw scene as we normally would to color texture 
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)



    renderBackground(settings); // Render your gradient or custom background
    
    if (settings.showDebugGrid)
        renderDebugPlaneGrid(projection, view);
    
    

    updateEditorPropertySettings();



    


    // draw skybox first
    if (m_skybox)
        m_skybox->draw(projection, view);  // this handles view matrix stripping translation inside




    blinnPhongShader.use();
    blinnPhongShader.setMat4("projection", projection);
    blinnPhongShader.setMat4("view", view);
    blinnPhongShader.setVec3("viewPos", camera->position);
    blinnPhongShader.setFloat("material.shadowIntensity", settings.shadowIntensity);
    blinnPhongShader.setInt("material.shadowCalculationMethod", static_cast<int>(settings.shadowCalculationMethod));
    blinnPhongShader.setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    blinnPhongShader.setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
    


    // should be moved in init !!!!!!!!!!!!!!!!!!!!!!
    // bind pre-computed area light LTC data
    glActiveTexture(GL_TEXTURE0 + U_LTC1);
    glBindTexture(GL_TEXTURE_2D, LTC1Map);
    glActiveTexture(GL_TEXTURE0 + U_LTC2);
    glBindTexture(GL_TEXTURE_2D, LTC2Map);
    //blinnPhongShader.setInt("LTC1", U_LTC1); // Tell the shader to use texture unit 20 for LTC1
    //blinnPhongShader.setInt("LTC2", U_LTC2); // Tell the shader to use texture unit 21 for LTC2


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


    // Resolve MSAA to screen or another texture FBO
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // render to framebuffer
    computeColorFramebuffer(settings);


    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::BlinnPhongRenderer::loadShaders()
{
    // blinn phong illumination model and lightning shader
    blinnPhongShader.init("blinnphong", "shaders/blinn-phong.vert", "shaders/blinn-phong.frag");

    // skybox reflection shader
    skyboxReflectionShader.init("cubemap", "shaders/cubemap.vert", "shaders/cubemap.frag");

	// gradient background shader
    backgroundShader.init("background", "shaders/blinnphong_background.vert", "shaders/blinnphong_background.frag");

    // shared shaders
    Renderer::loadShaders();
}

void engine::BlinnPhongRenderer::initBackground()
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

void engine::BlinnPhongRenderer::renderBackground(const SceneSettings& settings)
{
    // make sure we clear the framebuffer's content
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
    
    if (settings.backgroundGradientColors.enabled)
    {
        // gradient color background
        backgroundShader.use();
        backgroundShader.setVec4("topcolor", settings.backgroundGradientColors.topColor);
        backgroundShader.setVec4("bottomcolor", settings.backgroundGradientColors.bottomColor);
        backgroundShader.setFloat("ySplit", settings.backgroundGradientColors.ySplit);

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
    skyboxReflectionShader.clean();
    backgroundShader.clean();

	m_skybox->clean();
}