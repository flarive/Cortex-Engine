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

bool engine::OpenGLRenderer::bind(std::unique_ptr<Shader>& shader, std::vector<std::shared_ptr<Texture>>& textures, int baseUnit, bool resetHasTextures) const
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

void engine::OpenGLRenderer::unbind(std::vector<std::shared_ptr<Texture>>& textures, int baseUnit) const
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

engine::OpenGLRenderer::~OpenGLRenderer()
{
    logger.trace("OpenGLRenderer base destructor called");
}