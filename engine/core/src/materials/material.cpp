#include "../../include/materials/material.h"

#include "../../include/managers/texture_manager.h"

#include "../../include/managers/filesystem_manager.h"

#include <format>

using fsm = engine::FileSystemManager;


// BlinnPhong with textures/PBR with no combined textures ctor
engine::Material::Material(const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath)
    : m_diffuseTexPath(fsm::getFullPath(diffuseTexPath)), m_normalTexPath(fsm::getFullPath(normalTexPath)), m_heightTexPath(fsm::getFullPath(heightTexPath)), m_emissiveTexPath(fsm::getFullPath(emissiveTexPath)), m_opacityTexPath(opacityTexPath)
{
}

// PBR with combined textures ctor
engine::Material::Material(CombinedTexture mode, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath)
    : m_diffuseTexPath(fsm::getFullPath(diffuseTexPath)), m_normalTexPath(fsm::getFullPath(normalTexPath)), m_heightTexPath(fsm::getFullPath(heightTexPath)), m_emissiveTexPath(fsm::getFullPath(emissiveTexPath)), m_opacityTexPath(opacityTexPath)
{
}

std::string_view engine::Material::getUniformSamplerName(TextureSlot type) const
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

bool engine::Material::bind(std::unique_ptr<Shader>& shader, int baseUnit) const
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

    for (const auto& tex : m_textures)
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

bool engine::Material::bind2(std::unique_ptr<Shader>& shader) const
{
    unsigned int textureUnit = 0;
    bool success = true;

    shader->use();
    for (const auto& texture : m_textures)
    {
        const std::string_view uniformName = getUniformSamplerName(texture->type);
        
        if (texture->id > 0)
        {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texture->id);

            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
            {
                std::cerr << "OpenGL error while binding texture '" << uniformName << "' to unit " << textureUnit << ": " << std::hex << error << std::endl;
                success = false;
                continue;
            }

            shader->setInt(std::string(uniformName), textureUnit);
            textureUnit++;

            break;
        }
        else
        {
            //std::cerr << "Warning: Texture ID is 0 for '" << texture.type << "'. Texture might not be loaded correctly." << std::endl;
            //success = false;
            success = true;
        }
    }

    glActiveTexture(GL_TEXTURE0); // Reset active texture
    return success;
}

void engine::Material::unbind(int baseUnit) const
{
    unsigned int unit = baseUnit;
    for (const auto& tex : m_textures)
    {
        if (tex->id == 0)
            continue;

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D /* or tex.target */, 0);
        ++unit;
    }
    // No need to set back to GL_TEXTURE0
}

void engine::Material::setCubeMapTexs(const std::vector<std::string>& faces)
{
    m_cubemapTextures = faces;
}

bool engine::Material::isTransparent() const
{
    //// Smooth transparency (blending)
    //if (m_opacityIntensity < 1.0f)
    //    return true;

    // Opacity texture exists AND can produce alpha < 1
    if (hasOpacityMap())
    {
        return true;
        // If the opacity texture is used, transparency depends on intensity
        //if (m_opacityIntensity < 1.0f)
        //    return true;

        // If baseColorFactor.a < 1, also transparent
        //if (baseColorFactor.a < 1.0f)
        //    return true;
    }
    else
    {
        // Smooth transparency (blending)
        if (m_opacityIntensity < 1.0f)
            return true;
    }

    return false;
}

bool engine::Material::isAlphaCutout() const
{
    return m_alphaCutoffEnabled; // or opacityMode == MASK
}

void engine::Material::addTexture(std::shared_ptr<Texture> texture)
{
    if (!texture)
        return;

    m_textures.emplace_back(std::move(texture));
}

const std::shared_ptr<engine::Texture> engine::Material::getDiffuseTexture() const
{
    return engine::TextureManager::getTextureFromCache(m_diffuseTexPath);
}

const std::shared_ptr<engine::Texture> engine::Material::getNormalTexture() const
{
    return engine::TextureManager::getTextureFromCache(m_normalTexPath);
}

const std::shared_ptr<engine::Texture> engine::Material::getHeightTexture() const
{
    return engine::TextureManager::getTextureFromCache(m_heightTexPath);
}

const std::shared_ptr<engine::Texture> engine::Material::getEmissiveTexture() const
{
    return engine::TextureManager::getTextureFromCache(m_emissiveTexPath);
}

const std::shared_ptr<engine::Texture> engine::Material::getOpacityTexture() const
{
    return engine::TextureManager::getTextureFromCache(m_opacityTexPath);
}