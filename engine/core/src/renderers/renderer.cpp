#include "../../include/renderers/renderer.h"

#include "../../include/singleton.h"
#include "../../include/debug/opengl_debug.h"

#include <memory>


engine::Renderer::Renderer(GLFWwindow* window, bool useHDR)
    : m_window(window), m_useHDR(useHDR)
{
}


void engine::Renderer::loadShaders()
{
    // color framebuffer to screen shader
    screenShader.init("screen", "shaders/framebuffers_screen.vert", "shaders/framebuffers_screen.frag");

    // for spot lights or directional lights
    directionalDepthMapShader.init("simpleDepthBuffer1", "shaders/shadow_mapping_depth.vert", "shaders/shadow_mapping_depth.frag");

    // for point lights
    pointDepthMapShader.init("simpleDepthBuffer2", "shaders/point_shadow_depth.vert", "shaders/point_shadow_depth.frag", "shaders/point_shadow_depth.geom");


    // debug only
    depthMapToQuadShader.init("debugDepthQuad", "shaders/debug/debug_quad_depth.vert", "shaders/debug/debug_quad_depth.frag");
    cubeFaceDebugShader.init("debugDepthCube", "shaders/debug/cube_face_debugger.vert", "shaders/debug/cube_face_debugger.frag");

    // Editor mode outline shader
    outlineColorShader.init("outline", "shaders/debug/stencil_testing.vert", "shaders/debug/stencil_testing.frag");
}


void engine::Renderer::enableDepthTest(bool enable)
{
    // enable z buffer (depth test) to have correct objects depth ordering
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void engine::Renderer::enableStencilTest(bool enable)
{
    // enable stencil buffer (stencil test) to have objects outlining
    if (enable)
        glEnable(GL_STENCIL_TEST);
    else
        glDisable(GL_STENCIL_TEST);
}

void engine::Renderer::enableFaceCulling(bool enable)
{
    if (enable)
    {
        // optim : do not display hidden faces
        // consistent winding orders needed (counter-clockwise by default)
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void engine::Renderer::enableGammaCorrection(bool enable)
{
    // gamma correction (default 2.2 gamma correction)
    if (enable)
        glEnable(GL_FRAMEBUFFER_SRGB);
    else
        glDisable(GL_FRAMEBUFFER_SRGB);
}

void engine::Renderer::initDebugPlaneGrid()
{
    m_debugPlaneGrid.init(10, 0.5f);
}

void engine::Renderer::renderDebugPlaneGrid(const glm::mat4& projection, const glm::mat4& view)
{
    m_debugPlaneGrid.draw(projection, view);
}

void engine::Renderer::initDepthMapFramebuffer(GLsizei shadowSize)
{
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    if (m_lights.size() > 0)
    {
        auto firstLight = m_lights[0];
        if (std::dynamic_pointer_cast<PointLight>(firstLight))
            initPointLightDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);
        else
            initSpotLightDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);
    }
}

/// <summary>
/// Spotlight only !!!!
/// </summary>
void engine::Renderer::initSpotLightDepthMapFramebuffer(GLsizei shadowSize)
{
    // create depth framebuffer
    glGenFramebuffers(1, &depthMapFramebuffer);
    // create depth texture
    glGenTextures(1, &textureDepthMapBuffer);
    glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthMapBuffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shader configuration for visual debugging
    // -----------------------------------------
    //depthMapToQuadShader.use();
    //depthMapToQuadShader.setInt("depthMap", 0);
}

/// <summary>
/// Omnilight only !!!!
/// </summary>
void engine::Renderer::initPointLightDepthMapFramebuffer(GLsizei shadowSize)
{
    glGenFramebuffers(1, &depthMapFramebuffer);
    // create depth cubemap texture
    glGenTextures(1, &textureDepthMapBuffer);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureDepthMapBuffer);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowSize, shadowSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureDepthMapBuffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // shader configuration
    // --------------------
    //cubeFaceDebugShader.use();
    //cubeFaceDebugShader.setInt("uCubeMap", 0);
    //cubeFaceDebugShader.setInt("uFaceIndex", 1);
}

/// <summary>
/// Spotlight only !!!!!
/// </summary>
void engine::Renderer::computeDepthMapFramebuffer(Shader& shader, float width, float height, bool enableShadows, GLsizei shadowSize, std::function<void(Shader&)> update, std::shared_ptr<engine::Light> light)
{
    // 1. render depth of scene to texture (from light's perspective)
    // --------------------------------------------------------------
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 0.1f;  // Previously 1.0f
    float far_plane = 100.0f;  // Previously 7.5f
    lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
    lightView = glm::lookAt(light->position, light->target, glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

    // render scene from light's point of view
    directionalDepthMapShader.use();
    directionalDepthMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    

    glViewport(0, 0, (GLsizei)shadowSize, (GLsizei)shadowSize);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);


    glEnable(GL_POLYGON_OFFSET_FILL); // fix peter panning
    glPolygonOffset(2.0f, 4.0f); // Adjust these values to fine-tune shadow biasing
    update(directionalDepthMapShader);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    // 2. render scene as normal using the previously generated depth/shadow map  
    // -------------------------------------------------------------------------
    shader.use();
    shader.setVec3("lightPos", light->position);
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    shader.setBool("enableShadows", enableShadows);

    // update user stuffs
    update(shader);


    glActiveTexture(GL_TEXTURE0 + U_SHADOW_MAP);
    glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);
    shader.setInt("texture_shadowMap", U_SHADOW_MAP);



    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    //depthMapToQuadShader.use();
    //depthMapToQuadShader.setFloat("near_plane", near_plane);
    //depthMapToQuadShader.setFloat("far_plane", far_plane);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);


    //// test depth map (also comment computeColorFramebuffer);
    //renderQuad();
}

/// <summary>
/// Omnilight only !!!!!
/// </summary>
void engine::Renderer::computeDepthMapFramebuffer2(Shader& shader, float width, float height, bool enableShadows, GLsizei shadowSize, std::function<void(Shader&)> update, std::shared_ptr<engine::Light> light)
{
    // 0. create depth cubemap transformation matrices
    // -----------------------------------------------
    float near_plane = 1.0f;  // Previously 1.0f
    float far_plane = 25.0f;  // Previously 25.0f
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * glm::lookAt(light->position, light->position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light->position, light->position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light->position, light->position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light->position, light->position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light->position, light->position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(light->position, light->position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));



    // 1. render scene to depth cubemap
    // --------------------------------
    glViewport(0, 0, shadowSize, shadowSize);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    pointDepthMapShader.use();
    for (unsigned int i = 0; i < 6; ++i)
    {
        pointDepthMapShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    }
    pointDepthMapShader.setFloat("far_plane", far_plane);
    pointDepthMapShader.setVec3("lightPos", light->position);

    // update user stuffs
    update(pointDepthMapShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. render scene as normal 
    // -------------------------
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    glm::mat4 projection = m_camera->getProjectionMatrix(width, height, 0.1f, 100.0f);
    glm::mat4 view = m_camera->getViewMatrix();


    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setVec3("lightPos", light->position);
    shader.setVec3("viewPos", m_camera->position);
    shader.setBool("enableShadows", enableShadows);
    shader.setFloat("far_plane", far_plane);

    // update user stuffs
    update(shader);


    // not needed but need to be reserved to avoid conflicts or overrides
    //glActiveTexture(GL_TEXTURE0 + U_SHADOW_MAP);
    //glBindTexture(GL_TEXTURE_2D, 0);
    //shader.setInt("texture_shadowMap", U_SHADOW_MAP);

    glActiveTexture(GL_TEXTURE0 + U_SHADOW_MAP_CUBE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureDepthMapBuffer);
    shader.setInt("texture_shadowMapCube", U_SHADOW_MAP_CUBE);


    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    //cubeFaceDebugShader.use();
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, textureDepthMapBuffer);

    //// test depth map (also comment computeColorFramebuffer);
    //renderQuad();
}


void engine::Renderer::initColorFramebuffer(int width, int height)
{
    // create framebuffer
    glGenFramebuffers(1, &colorFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    // create a color attachment texture
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void engine::Renderer::initColorFramebufferMSAA(int width, int height)
{
    const int samples = 4; // Change this to your desired MSAA level

    // 1. Create multisampled framebuffer
    glGenFramebuffers(1, &colorFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);

    // 2. Create a multisampled color renderbuffer
    GLuint colorRBO;
    glGenRenderbuffers(1, &colorRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGB8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRBO);

    // 3. Create a multisampled depth+stencil renderbuffer
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // 4. Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR::FRAMEBUFFER:: MSAA Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void engine::Renderer::initHDRColorFramebufferMSAA(int width, int height)
{
    const int samples = 4;

    glGenFramebuffers(1, &colorFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);

    // MSAA color renderbuffer — use FLOATING POINT for HDR
    GLuint colorRBO = 0;
    glGenRenderbuffers(1, &colorRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, colorRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA16F, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRBO);

    // MSAA depth-stencil
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: MSAA HDR framebuffer incomplete\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create a non-MSAA floating-point COLOR TEXTURE to resolve into
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create an FBO that attaches the resolved texture (to receive blit)
    glGenFramebuffers(1, &resolveFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, resolveFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR: Resolve framebuffer incomplete\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}




void engine::Renderer::computeColorFramebuffer()
{
     //draw color framebuffer to screen
     //now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
    // clear all relevant buffers
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // set clear color (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);
    //screenShader.setBool("useHDR", false);
    //screenShader.setFloat("exposure", 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);	// use the color attachment texture as the texture of the quad plane
    
    // render HDR framebuffer to screen as a big fullscreen quad
    renderQuad();
}

void engine::Renderer::computeHDRColorFramebuffer(int width, int height)
{
    // Bind default framebuffer (usually SDR)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float exposure = 1.0f;

    screenShader.use();
    screenShader.setInt("screenTexture", 0);
    //screenShader.setBool("useHDR", true);
    //screenShader.setFloat("exposure", exposure); // e.g., 1.0–2.0

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer); // resolved non-MSAA float texture


    std::vector<float> data(width * height * 4);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data.data());

    // Sample the center pixel
    int x = width / 2;
    int y = height / 2;
    int idx = (y * width + x) * 4;

    float r = data[idx + 0];
    float g = data[idx + 1];
    float b = data[idx + 2];

    if (r > 1.0 || g > 1.0f || b > 1.0f)
        std::cout << "HDR test pixel YESSSSSSSSSSSSS = " << r << ", " << g << ", " << b << std::endl;

    // render HDR framebuffer to screen as a big fullscreen quad
    renderQuad();
}


void engine::Renderer::updateEditorPropertySettings()
{
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();
    
    
    // solid/wireframe polygons
    static bool lastRenderModeWireframe = settings.drawAsWireframe;
    if (lastRenderModeWireframe != settings.drawAsWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, settings.drawAsWireframe ? GL_LINE : GL_FILL);
        lastRenderModeWireframe = settings.drawAsWireframe;
    }

    // enable gamma correction
    static bool lastEnableGammaCorrection = settings.enableGammaCorrection;
    if (lastEnableGammaCorrection != settings.enableGammaCorrection)
    {
        enableGammaCorrection(settings.enableGammaCorrection);
        lastEnableGammaCorrection = settings.enableGammaCorrection;
    }
    
    // enable/disable back face culling
    static bool lastEnableFaceCulling = settings.enableFaceCulling;
    if (lastEnableFaceCulling != settings.enableFaceCulling)
    {
        enableFaceCulling(settings.enableFaceCulling);
        lastEnableFaceCulling = settings.enableFaceCulling;
    }

    // enable/disable camera frustrum culling
    static float lastShadowMapsTexturesize = settings.shadowMapsTextureSize;
    if (lastShadowMapsTexturesize != settings.shadowMapsTextureSize)
    {
        initDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);
        lastShadowMapsTexturesize = settings.shadowMapsTextureSize;
    }
}

void engine::Renderer::renderCube()
{
    // initialize (if necessary)
    if (m_cubeVAO == 0)
    {
        float vertices[] = {
            // back face (z = -1)
             1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left

            // front face (z = 1)
             1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
             1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

             // left face (x = -1)
             -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
             -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
             -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
             -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

             // right face (x = 1)
            1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f,  -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f,  -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,

            // bottom face (y = -1)
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

            // top face (y = 1)
             1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
             1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };


        glGenVertexArrays(1, &m_cubeVAO);
        glGenBuffers(1, &m_cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(m_cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Send to GPU
    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void engine::Renderer::renderQuad()
{
    if (m_quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &m_quadVBO);
        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    // Send to GPU
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void engine::Renderer::renderSphere()
{
    if (m_sphereVAO == 0)
    {
        glGenVertexArrays(1, &m_sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        m_indexCount = static_cast<GLsizei>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(m_sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    // Send to GPU
    glBindVertexArray(m_sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void engine::Renderer::clean()
{
    // delete shaders
    screenShader.clean();
	directionalDepthMapShader.clean();
    pointDepthMapShader.clean();
    depthMapToQuadShader.clean();
    cubeFaceDebugShader.clean();
    outlineColorShader.clean();

    // delete framebuffers
    glDeleteFramebuffers(1, &depthMapFramebuffer);
    glDeleteFramebuffers(1, &colorFramebuffer);

    // delete textures
    glDeleteTextures(1, &textureDepthMapBuffer);
    glDeleteTextures(1, &textureColorBuffer);

    // delete renderbuffer
    glDeleteRenderbuffers(1, &rbo);

    // delete cube VAO/VBO
    glDeleteVertexArrays(1, &m_cubeVAO);
    glDeleteBuffers(1, &m_cubeVBO);

    // delete quad VAO/VBO
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);

    // delete sphere VAO/VBO/IBO
    glDeleteVertexArrays(1, &m_sphereVAO);
}
