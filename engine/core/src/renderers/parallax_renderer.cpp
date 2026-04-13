#include "../../include/renderers/parallax_renderer.h"

#include "../../include/singleton.h"

#include "../../include/tools/file_system.h"
#include "../../include/debug/opengl_debug.h"
#include "../../include/debug/debug_frame.h"

#include "../../include/lights/spot_light.h"
#include "../../include/lights/point_light.h"
#include "../../include/lights/directional_light.h"

#include "../../include/misc/colors.h"




engine::ParallaxRenderer::ParallaxRenderer(GLFWwindow* window)
    : Renderer(window)
{}


void engine::ParallaxRenderer::setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights)
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
    //parallaxShader.use();
    //parallaxShader.setFloat("material.shadowIntensity", settings.shadowIntensity);
    //parallaxShader.setInt("material.shadowCalculationMethod", settings.shadowCalculationMethod);
    //parallaxShader.setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    //parallaxShader.setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
    


    // shader configuration
    // --------------------
    screenShader.use();
    screenShader.setInt("screenTexture", 0);

    // Depth map framebuffer configuration (for shadow map)
    // -----------------------------------
    initDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);

    // color framebuffer configuration
    // -------------------------
    initColorFramebufferMSAA(width, height); // SDR and AA
    //initColorFramebuffer(width, height); // SDR

    // solid/wireframe polygons
    //glPolygonMode(GL_FRONT_AND_BACK, settings.drawAsWireframe ? GL_LINE : GL_FILL);
}

void engine::ParallaxRenderer::loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&, Shader&)> update, std::function<void()> updateUI)
{
    //DebugFrame::ensureIsCalledOncePerFrame("BlinnPhongRenderer", "loop");

    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    glm::mat4 projection = camera->getProjectionMatrix(width * 1.0f / height * 1.0f);
    glm::mat4 view = camera->getViewMatrix();


    // bind to color framebuffer and draw scene as we normally would to color texture 
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)



    renderBackground(settings); // Render your gradient or custom background



    // Apply wireframe *only for the scene pass* if enabled
    glGetIntegerv(GL_POLYGON_MODE, m_prevPolyModes); // prevPolyModes[0]=front, [1]=back

    if (settings.drawAsWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);




    updateEditorPropertySettings();







    parallaxShader.use();
    parallaxShader.setMat4("projection", projection);
    parallaxShader.setMat4("view", view);
    parallaxShader.setVec3("viewPos", camera->position);
    //parallaxShader.setFloat("material.shadowIntensity", settings.shadowIntensity);
    //parallaxShader.setInt("material.shadowCalculationMethod", static_cast<int>(settings.shadowCalculationMethod));
    //parallaxShader.setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    //parallaxShader.setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);

    parallaxShader.setFloat("material.heightScale", 0.05f); // todo move in primitives
    //parallaxShader.setBool("material.useParallaxMapping", true); // todo move in primitives
    


    // update user stuffs
    update(parallaxShader, parallaxShader);

    // compute light shadows using a depth map framebuffer
    computeDepthMapFramebuffer(width, height, settings.enableShadows, (GLsizei)settings.shadowMapsTextureSize, parallaxShader, parallaxShader, update);


    // Resolve MSAA to screen or another texture FBO
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);



    // Restore whatever polygon mode was active before
    if (settings.drawAsWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, m_prevPolyModes[0]); // both front/back are same in core usage


    // render to framebuffer
    computeColorFramebuffer(settings);


    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::ParallaxRenderer::loadShaders()
{
    // blinn phong illumination model and lightning shader
    parallaxShader.init("parallax", "shaders/parallax_mapping.vert", "shaders/parallax_mapping.frag");
    
    // shared shaders
    Renderer::loadShaders();
}

void engine::ParallaxRenderer::renderBackground(const SceneSettings& settings)
{
    // solid color background
    Color backgroundColor = settings.backgroundColor;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void engine::ParallaxRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount)
{
    m_pointLightCount = pointLightCount;
    m_dirLightCount = dirLightCount;
    m_spotLightCount = spotLightCount;
    m_areaLightCount = areaLightCount;

    auto setCounts = [this](Shader& sh) {
        //sh.use();
        //sh.setInt("pointLightsCount", m_pointLightCount);
        //sh.setInt("dirLightsCount", m_dirLightCount);
        //sh.setInt("spotLightsCount", m_spotLightCount);
        //sh.setInt("areaLightsCount", m_areaLightCount);
        };

    setCounts(parallaxShader);
}

engine::Shader& engine::ParallaxRenderer::getShader()
{
    return parallaxShader;
}

void engine::ParallaxRenderer::clean()
{
    Renderer::clean();

    // delete shaders
    parallaxShader.clean();
}