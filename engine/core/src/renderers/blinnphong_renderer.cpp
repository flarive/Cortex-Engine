#include "../../include/renderers/blinnphong_renderer.h"


engine::BlinnPhongRenderer::BlinnPhongRenderer(GLFWwindow* window, const Camera& camera, std::vector<std::shared_ptr<engine::Light>> lights) : Renderer(window, camera, lights)
{
}


void engine::BlinnPhongRenderer::setup(const SceneSettings& settings, int width, int height)
{
    // configure global opengl state
    // -----------------------------
    enableDepthTest(true);
    enableFaceCulling(true);
    enableAntiAliasing(true);
    if (settings.applyGammaCorrection) enableGammaCorrection(true);


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


void engine::BlinnPhongRenderer::loop(int width, int height)
{
    // bind to color framebuffer and draw scene as we normally would to color texture 
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

    // make sure we clear the framebuffer's content
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update user stuffs
    //update(blinnPhongShader); ouchhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh

    // compute light shadows using a depth map framebuffer
    if (m_lights.size() > 0)
        computeDepthMapFramebuffer(blinnPhongShader, width, height, m_lights[0]);

    // render to framebuffer
    computeColorFramebuffer();

    // display UI/HUD above the scene and outside the framebuffer
    //updateUI(); ouchhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
}

void engine::BlinnPhongRenderer::loadShaders()
{
    // blinn phong illumination model and lightning shader
    blinnPhongShader.init("blinnphong", "shaders/blinn-phong.vertex", "shaders/blinn-phong.frag");

    pbrShader.init("pbr", "shaders/pbr.vertex", "shaders/pbr.frag");

    //Shader depthBufferShader("debug_depth_buffer", "shaders/debug/debug_depth_buffer.vertex", "shaders/debug/debug_depth_buffer.frag"); // depth buffer debugging shader

    // color framebuffer to screen shader
    screenShader.init("screen", "shaders/framebuffers_screen.vertex", "shaders/framebuffers_screen.frag");

    // skybox reflection shader
    skyboxReflectShader.init("cubemap", "shaders/cubemap.vertex", "shaders/cubemap.frag");

    simpleDepthShader.init("simpleDepthBuffer", "shaders/shadow_mapping_depth.vertex", "shaders/shadow_mapping_depth.frag");
    debugDepthQuad.init("debugDepthQuad", "shaders/debug/debug_quad_depth.vertex", "shaders/debug/debug_quad_depth.frag");



    // PBR
    equirectangularToCubemapShader.init("equirectangularToCubemapShader", "shaders/cubemap2.vertex", "shaders/equirectangular_to_cubemap.frag");
    irradianceShader.init("irradianceShader", "shaders/cubemap2.vertex", "shaders/irradiance_convolution.frag");
    prefilterShader.init("prefilterShader", "shaders/cubemap2.vertex", "shaders/prefilter.frag");
    brdfShader.init("brdfShader", "shaders/brdf.vertex", "shaders/brdf.frag");

    backgroundShader.init("background", "shaders/background.vertex", "shaders/background.frag");
}