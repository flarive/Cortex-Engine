#include "../../include/renderers/pbr_renderer.h"

#include "../../include/singleton.h"

#include "../../include/tools/file_system.h"
#include "../../include/debug/opengl_debug.h"

#include "../../include/lights/spot_light.h"
#include "../../include/lights/point_light.h"
#include "../../include/lights/directional_light.h"


#include "../../include/misc/colors.h"


engine::PbrRenderer::PbrRenderer(GLFWwindow* window)
    : Renderer(window)
{
}

/// <summary>
/// Query once per program startup and cache these caps, not every bind.
/// GLint maxFragUnits = 16;
/// glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);
/// </summary>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="camera"></param>
/// <param name="lights"></param>
void engine::PbrRenderer::setup(int width, int height, std::shared_ptr<Camera> camera, const std::vector<std::shared_ptr<Light>>& lights)
{
    m_lights = lights; // copy ?????????
    m_camera = camera; // copy ?????????

    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    // configure global opengl state
    // -----------------------------
    // enable depth testing
    enableDepthTest(true);
    // set depth function to less than AND equal for skybox depth trick.
    glDepthFunc(GL_LEQUAL);
    // enable seamless cubemap sampling for lower mip levels in the pre-filter map.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    // enable objects outlining
    enableStencilTest(true);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // avoid computing back faces not visible by camera
    enableFaceCulling(settings.enableFaceCulling);

    // build and compile shaders
    // -------------------------
    loadShaders();

    // generate on the fly textues in memory
    // -------------------------------------
    LTC1Map = Texture::loadMTexture();
    LTC2Map = Texture::loadLUTTexture();

    glm::mat4 projection = m_camera->getProjectionMatrix(width, height);



    // shader configuration
    // --------------------
    pbrShader.use();
    pbrShader.setMat4("projection", projection);
    pbrShader.setInt("material.texture_irradiance", U_IRR); // Should be texture unit, not texture ID
    pbrShader.setInt("material.texture_prefilter", U_PREF); // Should be texture unit, not texture ID
    pbrShader.setInt("material.texture_brdfLUT", U_BRDF); // Should be texture unit, not texture ID
    pbrShader.setInt("LTC1", U_LTC1); // Should be texture unit, not texture ID
    pbrShader.setInt("LTC2", U_LTC2); // Should be texture unit, not texture ID
    pbrShader.setFloat("material.shadowIntensity", settings.shadowIntensity);
    pbrShader.setInt("material.shadowCalculationMethod", settings.shadowCalculationMethod);
    pbrShader.setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    pbrShader.setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);
    pbrShader.setFloat("material.iblDiffuseIntensity", settings.iblDiffuseIntensity); // [0.0, 2.0]
    pbrShader.setFloat("material.iblSpecularIntensity", settings.iblSpecularIntensity); // [0.0, 5.0]



    screenShader.use();
    screenShader.setInt("screenTexture", 0); // Should be texture unit, not texture ID

    // Depth map framebuffer configuration (for shadow map)
    // -----------------------------------
    initDepthMapFramebuffer((GLsizei)settings.shadowMapsTextureSize);

    // color framebuffer configuration
    // -------------------------
    initHDRColorFramebufferMSAA(width, height); // HDR and AA
    //initColorFramebufferMSAA(width, height); // no HDR
    //initColorFramebuffer(width, height); // no AA

    // solid/wireframe polygons
    //glPolygonMode(GL_FRONT_AND_BACK, settings.drawAsWireframe ? GL_LINE : GL_FILL);


    int vsize{ 512 };
    //int scrWidth, scrHeight;
    //glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    //float qualityFactor = 2.0f; // 200% of the screen resolution
    //int vsize = static_cast<int>(std::max(scrWidth, scrHeight) * qualityFactor);


    if (settings.showDebugGrid)
        initDebugPlaneGrid();

    // pbr: setup framebuffer
    // ----------------------
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, vsize, vsize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    // Unbind both framebuffer and renderbuffer to restore state
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);



    // pbr: load the HDR environment map
    // ---------------------------------
    unsigned int hdrTexture = !settings.HDRSkyboxFilePath.empty() ? engine::Texture::loadHDRImage(FileSystem::getPath(settings.HDRSkyboxFilePath)) : 0;

    // pbr: setup cubemap to render to and attach to framebuffer
    // ---------------------------------------------------------
    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, vsize, vsize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    // ----------------------------------------------------------------------------------------------
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +X (right)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // -X (left)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)), // +Y (top)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)), // -Y (bottom)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)), // +Z (front)
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)) // -Z (back)
    };


    // pbr: convert HDR equirectangular environment map to cubemap equivalent
    // ----------------------------------------------------------------------
    equirectangularToCubemapShader.use();
    equirectangularToCubemapShader.setInt("equirectangularMap", 0);
    equirectangularToCubemapShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    glViewport(0, 0, vsize, vsize); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

    for (unsigned int i = 0; i < 6; ++i)
    {
        equirectangularToCubemapShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);




    // pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    glActiveTexture(GL_TEXTURE0); // Reset to default texture unit
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind the renderbuffer
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Unbind the cubemap texture




    // pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
    // -----------------------------------------------------------------------------
    irradianceShader.use();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }

    glActiveTexture(GL_TEXTURE0); // Reset active texture unit to default
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // Unbind the cubemap texture
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind renderbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
    glUseProgram(0); // Unbind shader program





    // pbr: create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale.
    // --------------------------------------------------------------------------------
    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minification filter to mip_linear 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    // ----------------------------------------------------------------------------------------------------
    prefilterShader.use();
    prefilterShader.setInt("environmentMap", 0);
    prefilterShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels{ 5 };
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // Resize framebuffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilterShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilterShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }

    
    glActiveTexture(GL_TEXTURE0); // Reset active texture unit to default
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind renderbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
    glUseProgram(0); // Unbind shader program




    // pbr: generate a 2D LUT from the BRDF equations used.
    // ----------------------------------------------------
    glGenTextures(1, &brdfLUTTexture);
    // Pre-allocate enough memory for the LUT texture.
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, vsize, vsize, 0, GL_RG, GL_FLOAT, 0);
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the 2D texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Re-configure capture framebuffer object and render screen-space quad with BRDF shader.
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, vsize, vsize);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

    glViewport(0, 0, vsize, vsize);
    brdfShader.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();

    glActiveTexture(GL_TEXTURE0); // Reset active texture unit to default
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
    glUseProgram(0); // Unbind shader program







    // initialize static shader uniforms before rendering
    // --------------------------------------------------
    //glm::mat4 projection = m_camera->getProjectionMatrix(width, height, 0.1f, 100.0f);

    //pbrShader.use();
    //pbrShader.setMat4("projection", projection);



    backgroundShader.use();
    backgroundShader.setInt("environmentMap", U_BG_ENV); // Should be texture unit, not texture ID
    backgroundShader.setMat4("projection", projection);


    // then before rendering, configure the viewport to the original framebuffer's screen dimensions
    int scrWidth{}, scrHeight{};
    glfwGetFramebufferSize(m_window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);
}

void engine::PbrRenderer::loop(int width, int height, std::shared_ptr<Camera> camera, std::function<void(Shader&)> update, std::function<void()> updateUI)
{
    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    const SceneSettings& settings = singleton->sceneSettings();

    // bind to color framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer(GL_FRAMEBUFFER, colorFramebuffer);
    glEnable(GL_DEPTH_TEST); // enable depth testing
    glEnable(GL_STENCIL_TEST); // enable stencil test


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // don't forget to clear the stencil buffer!

    updateEditorPropertySettings();

    glm::mat4 projection = camera->getProjectionMatrix(width, height);
    glm::mat4 view = camera->getViewMatrix();




    // Apply wireframe *only for the scene pass* if enabled
    glGetIntegerv(GL_POLYGON_MODE, m_prevPolyModes); // prevPolyModes[0]=front, [1]=back

    if (settings.drawAsWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    
    if (settings.showDebugGrid)
        renderDebugPlaneGrid(projection, view);


    



    // PBR shader
    pbrShader.use();
    pbrShader.setMat4("projection", projection);
    pbrShader.setMat4("view", view);
    pbrShader.setVec3("viewPos", camera->position);
    pbrShader.setFloat("material.shadowIntensity", settings.shadowIntensity);
    pbrShader.setInt("material.shadowCalculationMethod", static_cast<int>(settings.shadowCalculationMethod));
    pbrShader.setFloat("material.shadowMapsBias", settings.shadowMapsBiasFactor);
    pbrShader.setFloat("material.shadowMapsBlur", settings.shadowMapsBlur);


    // bind pre-computed IBL data
    glActiveTexture(GL_TEXTURE0 + U_IRR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glActiveTexture(GL_TEXTURE0 + U_PREF);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE0 + U_BRDF);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

    // bind pre-computed area light LTC data
    glActiveTexture(GL_TEXTURE0 + U_LTC1);
    glBindTexture(GL_TEXTURE_2D, LTC1Map);
    glActiveTexture(GL_TEXTURE0 + U_LTC2);
    glBindTexture(GL_TEXTURE_2D, LTC2Map);


    // update user stuffs
    update(pbrShader);
    //update(outlineColorShader);



    // render skybox (render as last to prevent overdraw)
    backgroundShader.use();
    backgroundShader.setMat4("view", view);
    backgroundShader.setMat4("projection", projection);
    backgroundShader.setFloat("blurStrength", settings.HDRSkyboxBlurStrength);

    // Bind the cube map texture to texture unit 0
    glActiveTexture(GL_TEXTURE0 + U_BG_ENV);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);


    if (!settings.HDRSkyboxHide)
        renderCube();


    // compute light shadows using a depth map framebuffer
    if (m_lights.size() > 0)
    {
        auto firstLight = m_lights[0];
        if (std::dynamic_pointer_cast<PointLight>(firstLight))
            computeDepthMapFramebuffer2(pbrShader, width, height, settings.enableShadows, (GLsizei)settings.shadowMapsTextureSize, update, firstLight);
        else
            computeDepthMapFramebuffer(pbrShader, width, height, settings.enableShadows, (GLsizei)settings.shadowMapsTextureSize, update, firstLight);
    }


    

    // Resolve MSAA to screen or another texture FBO (SDR old)
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Default framebuffer (screen)
    //glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    
    // Resolve MSAA color to colorFramebuffer (HDR)
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);




    // Restore whatever polygon mode was active before
    if (settings.drawAsWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, m_prevPolyModes[0]); // both front/back are same in core usage




    // render to framebuffer
    computeHDRColorFramebuffer(width, height, settings);
    //computeColorFramebuffer();

    // display UI/HUD above the scene and outside the framebuffer
    updateUI();
}

void engine::PbrRenderer::loadShaders()
{
    // PBR shaders
    pbrShader.init("pbr", "shaders/pbr.vert", "shaders/pbr.frag");
    equirectangularToCubemapShader.init("equirectangularToCubemapShader", "shaders/cubemap2.vert", "shaders/equirectangular_to_cubemap.frag");
    irradianceShader.init("irradianceShader", "shaders/cubemap2.vert", "shaders/irradiance_convolution.frag");
    prefilterShader.init("prefilterShader", "shaders/cubemap2.vert", "shaders/prefilter.frag");
    brdfShader.init("brdfShader", "shaders/brdf.vert", "shaders/brdf.frag");

    // HDR skybox shader
    backgroundShader.init("background", "shaders/pbr_background.vert", "shaders/pbr_background.frag");

    // shared shaders
    Renderer::loadShaders();
}

void engine::PbrRenderer::setLightsCount(unsigned short pointLightCount, unsigned short dirLightCount, unsigned short spotLightCount, unsigned int areaLightCount)
{
    m_pointLightCount = pointLightCount;
    m_dirLightCount = dirLightCount;
    m_spotLightCount = spotLightCount;
    m_areaLightCount = areaLightCount;

    pbrShader.use();
    pbrShader.setInt("pointLightsCount", m_pointLightCount);
    pbrShader.setInt("dirLightsCount", m_dirLightCount);
    pbrShader.setInt("spotLightsCount", m_spotLightCount);
    pbrShader.setInt("areaLightsCount", m_areaLightCount);
}

engine::Shader& engine::PbrRenderer::getShader()
{
    return pbrShader;
}

void engine::PbrRenderer::clean()
{
	Renderer::clean();
    
    // delete shaders
    pbrShader.clean();
    backgroundShader.clean();

    glDeleteFramebuffers(1, &captureFBO);
    glDeleteRenderbuffers(1, &captureRBO);
    glDeleteTextures(1, &envCubemap);
    glDeleteTextures(1, &irradianceMap);
    glDeleteTextures(1, &prefilterMap);
    glDeleteTextures(1, &brdfLUTTexture);
}