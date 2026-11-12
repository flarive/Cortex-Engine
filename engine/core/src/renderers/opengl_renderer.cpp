#include "../../include/renderers/opengl_renderer.h"


engine::OpenGLRenderer::OpenGLRenderer(GLFWwindow* window, RenderMethod method)
    : Renderer(window, method)
{
    logger.trace("OpenGLRenderer base constructor called");
}

void engine::OpenGLRenderer::loadShaders()
{
    // color framebuffer to screen shader
    screenShader = std::make_unique<OpenGLShader>("screen");
    screenShader->init("shaders/framebuffers_screen.vert", "shaders/framebuffers_screen.frag");

    // for spot lights or directional lights
    directionalDepthMapShader = std::make_unique<OpenGLShader>("simpleDepthBuffer1");
    directionalDepthMapShader->init("shaders/shadow_mapping_depth.vert", "shaders/shadow_mapping_depth.frag");

    directionalDepthMapTessellationShader = std::make_unique<OpenGLShader>("simpleDepthBuffer1Tess");
    directionalDepthMapTessellationShader->init("shaders/shadow_mapping_depth_tess.vert", "shaders/shadow_mapping_depth_tess.tcs", "shaders/shadow_mapping_depth_tess.tes", nullptr, "shaders/shadow_mapping_depth_tess.frag");

    // for point lights
    pointDepthMapShader = std::make_unique<OpenGLShader>("simpleDepthBuffer2");
    pointDepthMapShader->init("shaders/point_shadow_depth.vert", "shaders/point_shadow_depth.geom", "shaders/point_shadow_depth.frag");

    pointDepthMapTessellationShader = std::make_unique<OpenGLShader>("simpleDepthBuffer2Tess");
    pointDepthMapTessellationShader->init("shaders/point_shadow_depth.vert", "shaders/point_shadow_depth.geom", "shaders/point_shadow_depth.frag");

    // debug only
    depthMapToQuadShader = std::make_unique<OpenGLShader>("debugDepthQuad");
    depthMapToQuadShader->init("shaders/debug/debug_quad_depth.vert", "shaders/debug/debug_quad_depth.frag");

    cubeFaceDebugShader = std::make_unique<OpenGLShader>("debugDepthCube");
    cubeFaceDebugShader->init("shaders/debug/cube_face_debugger.vert", "shaders/debug/cube_face_debugger.frag");

    // Editor mode outline shader
    outlineColorShader = std::make_unique<OpenGLShader>("outline");
    outlineColorShader->init("shaders/debug/stencil_testing.vert", "shaders/debug/stencil_testing.frag");

    // Bloom shaders
    bloomBrightExtractionShader = std::make_unique<OpenGLShader>("brightExtraction");
    bloomBrightExtractionShader->init("shaders/postfx/bloom_bright_extraction.vert", "shaders/postfx/bloom_bright_extraction.frag");

    bloomBlurShader = std::make_unique<OpenGLShader>("bloomBlur");
    bloomBlurShader->init("shaders/postfx/bloom_blur.vert", "shaders/postfx/bloom_blur.frag");

    bloomFinalShader = std::make_unique<OpenGLShader>("bloomFinal");
    bloomFinalShader->init("shaders/postfx/bloom_combine_final.vert", "shaders/postfx/bloom_combine_final.frag");

    Renderer::loadShaders();
}

engine::OpenGLRenderer::~OpenGLRenderer()
{
    logger.trace("OpenGLRenderer base destructor called");
}