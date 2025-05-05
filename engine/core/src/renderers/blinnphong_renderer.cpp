#include "../../include/renderers/blinnphong_renderer.h"



void engine::BlinnPhongRenderer::setup()
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
    initColorFramebuffer();

    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_LINE
}