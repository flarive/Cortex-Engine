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

bool engine::OpenGLRenderer::bind(std::unique_ptr<Shader>& shader, const std::vector<std::shared_ptr<Texture>>& textures, int baseUnit, bool resetHasTextures) const
{
    static int m_maxFragUnits{};

    // Query once per program startup and cache these caps, not every bind
    if (m_maxFragUnits == 0)
    {
        GLint maxFragUnits = 16;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);
        m_maxFragUnits = maxFragUnits;
    }

    // Use shader once for the whole material
    shader->use();

    int unit = baseUnit;
    bool success = true;

    if (resetHasTextures)
    {
        // cleanup any previous bindings for this shader's material uniforms
        if (shader->getShaderType() == ShaderType::PBR || shader->getShaderType() == ShaderType::PBRTessellation)
        {
            shader->setBool("material.has_texture_diffuse_map", false);
            shader->setBool("material.has_texture_normal_map", false);
            shader->setBool("material.has_texture_metalness_map", false);
            shader->setBool("material.has_texture_roughness_map", false);
            shader->setBool("material.has_texture_ao_map", false);
            shader->setBool("materialHeight.has_texture_height_map", false);
            shader->setBool("material.has_texture_emissive_map", false);
            shader->setBool("material.has_texture_opacity_map", false);

            shader->setBool("material.has_texture_arm_map", false);
            shader->setBool("material.has_texture_rm_map", false);
        }
        else
        {
            shader->setBool("material.has_texture_diffuse_map", false);
            shader->setBool("material.has_texture_specular_map", false);
            shader->setBool("material.has_texture_normal_map", false);
            shader->setBool("materialHeight.has_texture_height_map", false);
            shader->setBool("material.has_texture_emissive_map", false);
            shader->setBool("material.has_texture_opacity_map", false);
        }
    }

    for (const auto& tex : textures)
    {
        const std::string_view uniformSamplerName = getUniformSamplerName(tex->type);

        // Example: tex.type could be "albedo", "normal", "metallic", ...
        std::string uniformName = std::format("material.{}", uniformSamplerName);
        std::string hasMapName = std::format("material.has_{}_map", uniformSamplerName);

        if (tex->type == TextureSlot::Height)
        {
            uniformName = std::format("materialHeight.{}", uniformSamplerName);
            hasMapName = std::format("materialHeight.has_{}_map", uniformSamplerName);
        }

        // If no texture, mark false and continue (don’t change unit).
        if (tex->id == 0)
        {
            shader->setBool(hasMapName, false);
            continue;
        }

        // Ensure we have room on the device
        if (unit >= m_maxFragUnits)
        {
            std::cerr << "[Material::bind] ERROR: Texture unit " << unit << " exceeds GL_MAX_TEXTURE_IMAGE_UNITS=" << m_maxFragUnits << " for uniform '" << uniformName << "'\n";
            shader->setBool(hasMapName, false);
            success = false;
            break; // or return false;
        }

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, tex->id);

#ifdef DEBUG
        // Keep this in debug only; glGetError is expensive
        if (GLenum error = glGetError(); error != GL_NO_ERROR)
        {
            std::cerr << std::hex << "[Material::bind] GL error 0x" << error << " binding '" << uniformSamplerName << "' to unit " << unit << " (target=" << target << ")\n" << std::dec;
            shader->setBool(hasMapName, false);
            success = false;
            // Continue to bind others, or break if you prefer fail-fast.
            ++unit;
            continue;
        }
#endif

        // Assign the sampler to the unit and flag presence
        shader->setInt(uniformName, unit);
        shader->setBool(hasMapName, true);

        ++unit;
    }

    // No need to reset glActiveTexture here.
    return success;
}

std::string_view engine::OpenGLRenderer::getUniformSamplerName(TextureSlot type) const
{
    switch (type)
    {
    case TextureSlot::Diffuse:  return "texture_diffuse";
    case TextureSlot::Specular: return "texture_specular";
    case TextureSlot::Normal:   return "texture_normal";
    case TextureSlot::Metallic: return "texture_metalness";
    case TextureSlot::Roughness:return "texture_roughness";
    case TextureSlot::AO:       return "texture_ao";
    case TextureSlot::Height:   return "texture_height";
    case TextureSlot::Emissive: return "texture_emissive";
    case TextureSlot::Opacity:  return "texture_opacity";
    case TextureSlot::ARM:      return "texture_arm";
    case TextureSlot::RM:       return "texture_rm";
    }

    return "texture_diffuse";
}

void engine::OpenGLRenderer::unbind(const std::vector<std::shared_ptr<Texture>>& textures, int baseUnit) const
{
    unsigned int unit = baseUnit;
    for (const auto& tex : textures)
    {
        if (tex->id == 0)
            continue;

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D /* or tex.target */, 0);
        ++unit;
    }
    // No need to set back to GL_TEXTURE0
}

void engine::OpenGLRenderer::renderModel(const std::shared_ptr<Model> model, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform)
{
    ShaderType type = shader->getShaderType();

    model->setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    shader->use();

    if (type == ShaderType::BlinnPhong || type == ShaderType::PBR || type == ShaderType::DepthBufferDirectionalLights || type == ShaderType::DepthBufferPointLights)
    {
        shader->setBool("isAnimated", model->hasBones());
    }

    if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
    {
        shader->setBool("isTessellated", false);
    }

    /*if (!m_shared_model)
    {*/
        for (auto& mesh : model->getMeshes()) {
            renderMesh(mesh, shader, transformMatrix);
        }
    //}
    //else
    //{
    //    // shared model, loaded one time, drawn multiple times
    //    for (auto& mesh : m_shared_model->getMeshes()) {
    //        mesh->draw(shader, projection, view, transformMatrix);
    //    }
    //}
}

void engine::OpenGLRenderer::renderMesh(const std::shared_ptr<Mesh> mesh, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix)
{
    ShaderType type = shader->getShaderType();

    auto material = mesh->getMaterial();
    if (!material)
        return;

    if (!material || !shader->isValid()) {
        std::cerr << "Material or shader not valid. Skipping draw." << std::endl;
        return;
    }

    if (!material->areAllTexturesLoaded()) {
        std::cout << "Textures not ready. Deferring draw." << std::endl;
        return;
    }

    if (mesh->getVAO() == 0 || mesh->getVBO() == 0) {
        std::cerr << "VAO/VBO not initialized. Skipping draw." << std::endl;
        return;
    }

    shader->use();
    OpenGLDebug::checkGLError("shader.use.mesh");

    if (material)
    {
        if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
        {
            if (!bind(shader, material->getTextures())) {
                std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
                return;
            }

            mesh->handleOpacity(shader);
            setShaderCommonUniforms(shader, material, type);

            // ???????????????????
            /*shader->setBool("material.canCastShadows", mesh->canCastShadows());
            shader->setBool("material.canReceiveShadows", mesh->canReceiveShadows());*/
        }


        // used by all shaders (blinnphong, pbr, simpleDepthBuffer1, simpleDepthBuffer2)
        glm::mat4 finalTransform = transformMatrix;

        

        if (!mesh->isSkinned())
            finalTransform *= mesh->getNodeTransform();

        shader->setMat4("model", finalTransform);

        if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
        {
            shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(finalTransform))));
            shader->setBool("hasTangents", mesh->hasTangents()); // depends on loaded model and model format
        }

        // Send to GPU
        glBindVertexArray(mesh->getVAO());
        OpenGLDebug::checkGLError("glBindVertexArray.mesh");


        bool wasFaceCullingEnabled = RenderState::isFaceCullingEnabled();

        // disable face culling for transparent primitives (to see back faces)
        if (type == ShaderType::PBR)
        {
            if (material->isTransparent() || material->hasTransmission())
                RenderState::disableFaceCulling();
        }

        glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
        OpenGLDebug::checkGLError("glDrawArrays.mesh");

        // re enable face culling for transparent primitives if face culling was enabled
        if (wasFaceCullingEnabled)
            RenderState::enableFaceCulling();
        else
            RenderState::disableFaceCulling();

        glBindVertexArray(0);
        OpenGLDebug::checkGLError("glBindVertexArray.mesh");

        // draw normals for debug
        //SceneSettings& sceneSettings = engine::Singleton::getInstance()->sceneSettings();

        //if (sceneSettings.drawNormalsVisualHelpers) {
        //    drawDebugNormals(projection, view, transformMatrix);
        //}

        if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
        {
            unbind(material->getTextures()); // Unbind textures to prevent OpenGL state retention
            OpenGLDebug::checkGLError("Unbind.mesh");
        }

        // restore opacity defaults
        RenderState::setOpaque();
    }
}

void engine::OpenGLRenderer::renderPrimitive(const std::shared_ptr<Primitive> primitive, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform)
{
    ShaderType type = shader->getShaderType();

    auto material = primitive->getMaterial();
    if (!material)
        return;

    if (!material || !shader->isValid()) {
        std::cerr << "Material or shader not valid. Skipping draw." << std::endl;
        return;
    }

    if (!material->areAllTexturesLoaded()) {
        std::cout << "Textures not ready. Deferring draw." << std::endl;
        return;
    }

    if (primitive->getVAO() == 0 || primitive->getVBO() == 0) {
        std::cerr << "VAO/VBO not initialized. Skipping draw." << std::endl;
        return;
    }

    shader->use();
    OpenGLDebug::checkGLError("shader.use.primitive");

    primitive->setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    if (material)
    {
        if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
        {
            if (!bind(shader, material->getTextures())) {
                std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
                return;
            }

            primitive->handleOpacity(shader);
            setShaderCommonUniforms(shader, material, type);

            shader->setBool("material.canCastShadows", primitive->canCastShadows());
            shader->setBool("material.canReceiveShadows", primitive->canReceiveShadows());
        }
    }
    
    // used by all shaders (blinnphong, pbr, simpleDepthBuffer1, simpleDepthBuffer2)
    if (type != ShaderType::Phong)
    {
        // old way not using UBOs
        shader->setMat4("model", transformMatrix);
    }
    else
    {
        // new OpenGL/Vulkan shared way using UBOS
        PhongModelUBO phongModelUBO{ transformMatrix };
        shader->updateUniformBuffer("PhongModelUBO", &phongModelUBO, sizeof(PhongModelUBO));
    }


    if (type == ShaderType::BlinnPhong || type == ShaderType::PBR)
    {
        shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transformMatrix))));
        shader->setBool("hasTangents", true);
        shader->setBool("isAnimated", false);
        shader->setBool("isTessellated", primitive->isTessellated());
    }

    // Send to GPU
    glBindVertexArray(primitive->getVAO());
    OpenGLDebug::checkGLError("glBindVertexArray.primitive");

    bool wasFaceCullingEnabled = RenderState::isFaceCullingEnabled();

    // disable face culling for transparent primitives (to see back faces)
    if (material->isTransparent() || material->hasTransmission())
        RenderState::disableFaceCulling();

    if (primitive->getTypeID() == PrimitiveType::triangle)
    {
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    else if (primitive->getTypeID() == PrimitiveType::plane || primitive->getTypeID() == PrimitiveType::billboard)
    {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    else if (primitive->getTypeID() == PrimitiveType::cube)
    {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    else if (primitive->getTypeID() == PrimitiveType::sphere || primitive->getTypeID() == PrimitiveType::cone || primitive->getTypeID() == PrimitiveType::cylinder)
    {
        glDrawElements(GL_TRIANGLE_STRIP, primitive->getIndexCount(), GL_UNSIGNED_INT, 0);
    }





    OpenGLDebug::checkGLError("glDrawElements.primitive");

    // re enable face culling for transparent primitives if face culling was enabled
    if (wasFaceCullingEnabled)
        RenderState::enableFaceCulling();
    else
        RenderState::disableFaceCulling();

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray.primitive");

    if (material && (type == ShaderType::BlinnPhong || type == ShaderType::PBR))
    {
        unbind(material->getTextures()); // Unbind textures to prevent OpenGL state retention
        OpenGLDebug::checkGLError("Unbind.primitive");
    }

    // restore opacity defaults
    RenderState::setOpaque();
}

void engine::OpenGLRenderer::renderParticleSystem(const std::shared_ptr<ParticleSystem> primitive, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform, const glm::mat4& projection, const glm::mat4& view)
{
    ShaderType type = shader->getShaderType();

    auto material = primitive->getMaterial();
    if (!material)
        return;

    if (type != ShaderType::BlinnPhong && type != ShaderType::PBR) {
        return;
    }

    if (!material || !shader->isValid()) {
        std::cerr << "Material or shader not valid. Skipping draw." << std::endl;
        return;
    }

    if (!material->areAllTexturesLoaded()) {
        std::cout << "Textures not ready. Deferring draw." << std::endl;
        return;
    }

    if (primitive->getVAO() == 0 || primitive->getVBO() == 0) {
        std::cerr << "VAO/VBO not initialized. Skipping draw." << std::endl;
        return;
    }

    primitive->prep();
    
    primitive->setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    auto particleSystemType = primitive->getParticleSystemType();

    if (particleSystemType == ParticleSystemType::basic)
    {
        auto& shaderSourceBasic = primitive->getShaderSourceBasic();
        
        if (!bind(shaderSourceBasic, material->getTextures(), Material::MATERIAL_BASE_UNIT, false)) {
            std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
            return;
        }

        shaderSourceBasic->setMat4("model", transformMatrix);
        shaderSourceBasic->setMat4("view", view);
        shaderSourceBasic->setMat4("projection", projection);
    }
    else if (particleSystemType == ParticleSystemType::geometry)
    {
        auto& shaderSourceGeometry = primitive->getShaderSourceGeometry();

        if (!bind(shaderSourceGeometry, material->getTextures(), Material::MATERIAL_BASE_UNIT, false)) {
            std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
            return;
        }

        shaderSourceGeometry->setMat4("model", transformMatrix);
        shaderSourceGeometry->setMat4("view", view);
        shaderSourceGeometry->setMat4("projection", projection);
        shaderSourceGeometry->setFloat("squareSize", primitive->getSquareSize());
    }
    else if (particleSystemType == ParticleSystemType::instanced)
    {
        auto& shaderSourceInstance = primitive->getShaderSourceInstanced();

        if (!bind(shaderSourceInstance, material->getTextures(), Material::MATERIAL_BASE_UNIT, false)) {
            std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
            return;
        }

        shaderSourceInstance->setMat4("model", transformMatrix);
        shaderSourceInstance->setMat4("view", view);
        shaderSourceInstance->setMat4("projection", projection);
    }

    RenderState::disableDepthMask();
    RenderState::enableBlending();
    RenderState::setBlendFunc(GL_SRC_ALPHA, GL_ONE);


    if (particleSystemType == ParticleSystemType::basic)
    {
        // Send to GPU
        glBindVertexArray(primitive->getVAO());
        OpenGLDebug::checkGLError("glBindVertexArray");

        int numOfSquares = primitive->getCurrentDataSize() / 8;
        glDrawElements(GL_TRIANGLES, numOfSquares * 6, GL_UNSIGNED_INT, 0);
        OpenGLDebug::checkGLError("glDrawElements");

        primitive->incrementDrawCalls(); // Increment for each draw call
    }
    else if (particleSystemType == ParticleSystemType::geometry)
    {
        // Send to GPU
        glBindVertexArray(primitive->getVAO());
        OpenGLDebug::checkGLError("glBindVertexArray");

        glDrawArrays(GL_POINTS, 0, primitive->getCurrentDataSize());
        OpenGLDebug::checkGLError("glDrawArrays");

        primitive->incrementDrawCalls(); // Increment for each draw call
    }
    else if (particleSystemType == ParticleSystemType::instanced)
    {
        // Send to GPU
        glBindVertexArray(primitive->getQuadVAO());
        OpenGLDebug::checkGLError("glBindVertexArray");

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, primitive->getCurrentDataSize());
        OpenGLDebug::checkGLError("glDrawArraysInstanced");

        primitive->incrementDrawCalls(); // Increment for each draw call
    }

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray");

    unbind(material->getTextures()); // Unbind textures to prevent OpenGL state retention
    OpenGLDebug::checkGLError("Unbind");

    RenderState::setDepthWrite(true);
}

void engine::OpenGLRenderer::renderTerrain(const std::shared_ptr<Terrain> primitive, std::unique_ptr<Shader>& shader, const glm::mat4& transformMatrix, const Transform& localTransform)
{
    ShaderType type = shader->getShaderType();

    auto material = primitive->getMaterial();
    if (!material)
        return;

    if (!material || !shader->isValid()) {
        std::cerr << "Material or shader not valid. Skipping draw." << std::endl;
        return;
    }

    if (!material->areAllTexturesLoaded()) {
        std::cout << "Textures not ready. Deferring draw." << std::endl;
        return;
    }

    if (primitive->getVAO() == 0 || primitive->getVBO() == 0) {
        std::cerr << "VAO/VBO not initialized. Skipping draw." << std::endl;
        return;
    }

    shader->use();
    OpenGLDebug::checkGLError("shader.use.terrain");

    primitive->setTransform(localTransform.getLocalPosition(), localTransform.getLocalRotation(), localTransform.getLocalScale());

    if (type == ShaderType::BlinnPhongTessellation || type == ShaderType::PBRTessellation)
    {
        if (!bind(shader, material->getTextures())) {
            std::cerr << "Failed to bind textures. Skipping draw." << std::endl;
            return;
        }

        setShaderCommonUniforms(shader, material, type);

        shader->setBool("material.canCastShadows", primitive->canCastShadows());
        shader->setBool("material.canReceiveShadows", primitive->canReceiveShadows());
    }
    else if (type == ShaderType::DepthBufferDirectionalLightsTessellation)
    {
        std::shared_ptr<Texture> heightTexture = material->getHeightTexture();
        if (heightTexture && heightTexture->id > 0)
        {
            shader->setInt("materialHeight.texture_height", heightTexture->id);
            shader->setBool("materialHeight.has_texture_height_map", true);
        }
        else
        {
            shader->setInt("materialHeight.texture_height", 0);
            shader->setBool("materialHeight.has_texture_height_map", false);
        }
    }

    shader->setMat4("model", transformMatrix);

    if (type == ShaderType::BlinnPhongTessellation || type == ShaderType::PBRTessellation)
    {
        shader->setBool("hasTangents", true);
        shader->setBool("isAnimated", false);
        shader->setBool("isTessellated", primitive->isTessellated());
    }

    if (type == ShaderType::BlinnPhongTessellation || type == ShaderType::PBRTessellation || type == ShaderType::DepthBufferDirectionalLightsTessellation)
    {
        shader->setFloat("heightFactor", primitive->getHeightFactor());
        shader->setVec2("heightOffset", primitive->getHeightOffset());

        if (type == ShaderType::BlinnPhongTessellation || type == ShaderType::PBRTessellation)
        {
            shader->setFloat("heightTextureSize", static_cast<float>(primitive->getTextureWidth()));
        }
    }


    // render the terrain
    glBindVertexArray(primitive->getVAO());
    OpenGLDebug::checkGLError("glBindVertexArray.terrain");

    glDrawArrays(GL_PATCHES, 0, static_cast<GLsizei>(primitive->getVerticeCount()));
    OpenGLDebug::checkGLError("glDrawArrays.terrain");

    glBindVertexArray(0);
    OpenGLDebug::checkGLError("glBindVertexArray.terrain");

    if (type == ShaderType::BlinnPhongTessellation || type == ShaderType::PBRTessellation)
    {
        unbind(material->getTextures()); // Unbind textures to prevent OpenGL state retention
        OpenGLDebug::checkGLError("Unbind.terrain");
    }
}

void engine::OpenGLRenderer::setShaderCommonUniforms(std::unique_ptr<Shader>& shader, std::shared_ptr<Material>& material, ShaderType type)
{
    std::shared_ptr<BlinnPhongMaterial> blinnPhongMaterial{};
    std::shared_ptr<PBRMaterial> pbrMaterial{};

    if (material->getTypeID() == MaterialType::PBR)
        pbrMaterial = std::static_pointer_cast<PBRMaterial>(material);
    else
        blinnPhongMaterial = std::static_pointer_cast<BlinnPhongMaterial>(material);

    if (type == ShaderType::PBR || type == ShaderType::PBRTessellation)
    {
        shader->setVec3("material.baseColorFactor", pbrMaterial->getBaseColorFactor());
        shader->setFloat("material.roughnessMapIntensity", pbrMaterial->getRougnessIntensity());
        shader->setFloat("material.metallicMapIntensity", pbrMaterial->getMetallicIntensity());
        shader->setFloat("material.ambientOcclusionMapIntensity", pbrMaterial->getAmbientOcclusionIntensity());

        shader->setFloat("material.IOR", pbrMaterial->getIOR());
        shader->setFloat("material.transmission", pbrMaterial->getTransmission());
        shader->setFloat("material.thickness", pbrMaterial->getThickness());
        shader->setVec3("material.attenuationColor", pbrMaterial->getAttenuationColor());
        shader->setFloat("material.attenuationDistance", pbrMaterial->getAttenuationDistance());
    }
    else  if (type == ShaderType::BlinnPhong || type == ShaderType::BlinnPhongTessellation)
    {
        shader->setFloat("material.specularMapIntensity", blinnPhongMaterial->getSpecularIntensity());
        shader->setFloat("material.shininess", blinnPhongMaterial->getShininessIntensity());
        shader->setVec3("material.ambient_color", blinnPhongMaterial->getAmbientColor());
        shader->setVec3("material.diffuse_color", blinnPhongMaterial->getDiffuseColor());
        shader->setVec3("material.specular_color", blinnPhongMaterial->getSpecularColor());
    }

    shader->setBool("material.useParallaxMapping", material->useParallaxMapping());
    shader->setFloat("material.parallaxMapIntensity", material->getParallaxIntensity());

    shader->setFloat("material.normalMapIntensity", material->getNormalIntensity());
    shader->setFloat("material.emissiveStrength", material->getEmissiveStrength());
    shader->setVec3("material.emissiveColor", material->getEmissiveColorFactor());

    


    shader->setFloat("material.opacity", material->getOpacityIntensity());
}


void engine::OpenGLRenderer::uploadPrimitive(const std::shared_ptr<Primitive> primitive)
{
    //glGenVertexArrays(1, &m_VAO);
    //glGenBuffers(1, &m_VBO);
    //glGenBuffers(1, &m_EBO);
}


engine::OpenGLRenderer::~OpenGLRenderer()
{
    logger.trace("OpenGLRenderer base destructor called");
}