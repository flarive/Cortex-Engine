#include "../../include/materials/material.h"

#include <format>


#define NDEBUG // to remove !!!!!!!!!!!!!!!



engine::Material::Material(std::vector<Texture> _textures, float _shininess)
    : textures(std::move(_textures)), m_shininess(_shininess)
{

}

engine::Material::Material(const Color& ambientColor)
    : m_ambientColor(ambientColor), m_diffuseTexPath(""), m_specularTexPath(""), m_normalTexPath(""), m_metallicTexPath(""), m_roughnessTexPath(""), m_aoTexPath(""), m_heightTexPath(""), m_shininess(0.0f)
{

}

engine::Material::Material(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess)
    : m_ambientColor(ambientColor), m_diffuseColor(diffuseColor), m_specularColor(specularColor), m_shininess(shininess)
{

}

engine::Material::Material(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath, const std::string& heightTexPath, float shininess)
    : m_ambientColor(ambientColor), m_diffuseTexPath(diffuseTexPath), m_specularTexPath(specularTexPath), m_normalTexPath(normalTexPath), m_metallicTexPath(metallicTexPath), m_roughnessTexPath(roughnessTexPath), m_aoTexPath(aoTexPath), m_heightTexPath(heightTexPath), m_shininess(shininess)
{

}

//bool engine::Material::bind(engine::Shader& shader, unsigned int baseUnit) const
//{
//    unsigned int textureUnit = 0;
//    bool success = true;
//
//    shader.use();
//    for (const auto& texture : textures)
//    {
//        const std::string uniformName = std::format("material.{}", texture.type);
//        const std::string hasMapName = std::format("material.has_{}_map", texture.type);
//
//        if (texture.id > 0)
//        {
//            glActiveTexture(GL_TEXTURE0 + textureUnit);
//            glBindTexture(GL_TEXTURE_2D, texture.id);
//
//            GLenum error = glGetError();
//            if (error != GL_NO_ERROR)
//            {
//                std::cerr << "OpenGL error while binding texture '" << texture.type
//                    << "' to unit " << textureUnit << ": " << std::hex << error << std::endl;
//                shader.setBool(hasMapName, false);
//                success = false;
//                continue;
//            }
//
//            shader.setInt(uniformName, textureUnit);
//            shader.setBool(hasMapName, true);
//            textureUnit++;
//        }
//        else
//        {
//            //std::cerr << "Warning: Texture ID is 0 for '" << texture.type << "'. Texture might not be loaded correctly." << std::endl;
//            shader.setBool(hasMapName, false);
//            //success = false;
//            success = true;
//        }
//    }
//
//    glActiveTexture(GL_TEXTURE0); // Reset active texture
//    return success;
//}

bool engine::Material::bind(engine::Shader& shader, unsigned int baseUnit) const
{
    // Query once per program startup and cache these caps, not every bind. TODO cache it !!!!!!!!!!!
    GLint maxFragUnits = 16;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragUnits);

    // Use shader once for the whole material
    shader.use();

    unsigned int unit = baseUnit;
    bool success = true;

    for (const auto& tex : textures)
    {
        // Example: tex.type could be "albedo", "normal", "metallic", ...
        const std::string uniformName = std::format("material.{}", tex.type);
        const std::string hasMapName = std::format("material.has_{}_map", tex.type);

        // If no texture, mark false and continue (don’t change unit).
        if (tex.id == 0)
        {
            shader.setBool(hasMapName, false);
            continue;
        }

        // Ensure we have room on the device
        if (unit >= maxFragUnits)
        {
            std::cerr << "[Material::bind] ERROR: Texture unit " << unit
                << " exceeds GL_MAX_TEXTURE_IMAGE_UNITS=" << maxFragUnits
                << " for uniform '" << uniformName << "'\n";
            shader.setBool(hasMapName, false);
            success = false;
            break; // or return false;
        }

        // Bind using the right target. If you only use 2D textures for materials, keep GL_TEXTURE_2D.
        GLenum target = GL_TEXTURE_2D; // TODO: extend your Texture to store its target.
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(target, tex.id);

#ifndef NDEBUG
        // Keep this in debug only; glGetError is expensive
        if (GLenum error = glGetError(); error != GL_NO_ERROR)
        {
            std::cerr << std::hex
                << "[Material::bind] GL error 0x" << error
                << " binding '" << tex.type << "' to unit " << unit
                << " (target=" << target << ")\n" << std::dec;
            shader.setBool(hasMapName, false);
            success = false;
            // Continue to bind others, or break if you prefer fail-fast.
            ++unit;
            continue;
        }
#endif

        // Assign the sampler to the unit and flag presence
        shader.setInt(uniformName, static_cast<int>(unit));
        shader.setBool(hasMapName, true);

        ++unit;
    }

    // No need to reset glActiveTexture here.
    return success;
}

bool engine::Material::bind2(engine::Shader& shader) const
{
    unsigned int textureUnit = 0;
    bool success = true;

    shader.use();
    for (const auto& texture : textures)
    {
        const std::string uniformName = std::format("{}", texture.type);

        if (texture.id > 0)
        {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texture.id);

            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
            {
                std::cerr << "OpenGL error while binding texture '" << texture.type
                    << "' to unit " << textureUnit << ": " << std::hex << error << std::endl;
                success = false;
                continue;
            }

            shader.setInt(uniformName, textureUnit);
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

//void engine::Material::unbind() const
//{
//    for (size_t i = 0; i < textures.size(); ++i)
//    {
//        if (textures[i].id > 0)
//        {
//            glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));
//            glBindTexture(GL_TEXTURE_2D, 0);
//
//            GLenum error = glGetError();
//            if (error != GL_NO_ERROR)
//            {
//                std::cerr << "OpenGL error while unbinding texture unit " << i << ": " << std::hex << error << std::endl;
//            }
//        }
//    }
//
//    glActiveTexture(GL_TEXTURE0); // Reset to default
//}

void engine::Material::unbind(int baseUnit) const
{
    unsigned int unit = baseUnit;
    for (const auto& tex : textures)
    {
        if (tex.id == 0)
            continue;

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D /* or tex.target */, 0);
        ++unit;
    }
    // No need to set back to GL_TEXTURE0
}

void engine::Material::loadTextures()
{
    textures.clear();  // Prevent duplicates

    unsigned int diffuseMapId = hasDiffuseMap() ? engine::Texture::loadTexture(getDiffuseTexPath(), true, false) : 0;
    textures.emplace_back(std::move(engine::Texture{ diffuseMapId, "texture_diffuse", getDiffuseTexPath() }));

    unsigned int specularMapId = hasSpecularMap() ? engine::Texture::loadTexture(getSpecularTexPath(), true, false) : 0;
    textures.emplace_back(std::move(engine::Texture{ specularMapId, "texture_specular", getSpecularTexPath() }));

    unsigned int normalMapId = hasNormalMap() ? engine::Texture::loadTexture(getNormalTexPath(), true, false) : 0;
    textures.emplace_back(std::move(engine::Texture{ normalMapId, "texture_normal", getNormalTexPath() }));

    unsigned int metallicMapId = hasMetallicMap() ? engine::Texture::loadTexture(getMetallicTexPath(), true, false) : 0;
    textures.emplace_back(std::move(engine::Texture{ metallicMapId, "texture_metalness", getMetallicTexPath() }));

    unsigned int roughnessMapId = hasRoughnessMap() ? engine::Texture::loadTexture(getRoughnessTexPath(), true, false) : 0;
    textures.emplace_back(std::move(engine::Texture{ roughnessMapId, "texture_roughness", getRoughnessTexPath() }));

    unsigned int aoMapId = hasAoMap() ? engine::Texture::loadTexture(getAoTexPath(), true, false) : 0;
    textures.emplace_back(std::move(engine::Texture{ aoMapId, "texture_ao", getAoTexPath() }));

    unsigned int heightMapId = hasHeightMap() ? engine::Texture::loadTexture(getHeightTexPath(), true, false) : 0;
    textures.emplace_back(std::move(engine::Texture{ heightMapId, "texture_height", getHeightTexPath() }));

    unsigned int emissiveMapId = hasEmissiveMap() ? engine::Texture::loadTexture(getEmissiveTexPath(), true, false) : 0;
    textures.emplace_back(std::move(engine::Texture{ emissiveMapId, "texture_emissive", getEmissiveTexPath() }));
}


void engine::Material::loadTexturesAsync()
{
    textures.clear();
    textures.reserve(7);

    // Load textures asynchronously
    engine::Texture::loadTextureAsync(getDiffuseTexPath());
    engine::Texture::loadTextureAsync(getSpecularTexPath());
    engine::Texture::loadTextureAsync(getNormalTexPath());
    engine::Texture::loadTextureAsync(getMetallicTexPath());
    engine::Texture::loadTextureAsync(getRoughnessTexPath());
    engine::Texture::loadTextureAsync(getAoTexPath());
    engine::Texture::loadTextureAsync(getHeightTexPath());
    engine::Texture::loadTextureAsync(getEmissiveTexPath());


    // Queue OpenGL execution on main thread
    unsigned int diffuseMapId = hasDiffuseMap() ? engine::Texture::enqueueTextureCreation(getDiffuseTexPath(), true) : 0;
    unsigned int specularMapId = hasSpecularMap() ? engine::Texture::enqueueTextureCreation(getSpecularTexPath(), true) : 0;
    unsigned int normalMapId = hasNormalMap() ? engine::Texture::enqueueTextureCreation(getNormalTexPath(), true) : 0;
    unsigned int metallicMapId = hasMetallicMap() ? engine::Texture::enqueueTextureCreation(getMetallicTexPath(), true) : 0;
    unsigned int roughnessMapId = hasRoughnessMap() ? engine::Texture::enqueueTextureCreation(getRoughnessTexPath(), true) : 0;
    unsigned int aoMapId = hasAoMap() ? engine::Texture::enqueueTextureCreation(getAoTexPath(), true) : 0;
    unsigned int heightMapId = hasHeightMap() ? engine::Texture::enqueueTextureCreation(getHeightTexPath(), true) : 0;
    unsigned int emissiveMapId = hasEmissiveMap() ? engine::Texture::enqueueTextureCreation(getEmissiveTexPath(), true) : 0;

    // process queue
    engine::Texture::processLoadedTextures();

    // get TextureID from queue
    diffuseMapId = engine::TextureManager::textureIDCache[getDiffuseTexPath()];
    textures.emplace_back(std::move(engine::Texture{ diffuseMapId, "texture_diffuse", getDiffuseTexPath() }));

    specularMapId = engine::TextureManager::textureIDCache[getSpecularTexPath()];
    textures.emplace_back(std::move(engine::Texture{ specularMapId, "texture_specular", getSpecularTexPath() }));

    normalMapId = engine::TextureManager::textureIDCache[getNormalTexPath()];
    textures.emplace_back(std::move(engine::Texture{ normalMapId, "texture_normal", getNormalTexPath() }));

    metallicMapId = engine::TextureManager::textureIDCache[getMetallicTexPath()];
    textures.emplace_back(std::move(engine::Texture{ metallicMapId, "texture_metalness", getMetallicTexPath() }));

    roughnessMapId = engine::TextureManager::textureIDCache[getRoughnessTexPath()];
    textures.emplace_back(std::move(engine::Texture{ roughnessMapId, "texture_roughness", getRoughnessTexPath() }));

    aoMapId = engine::TextureManager::textureIDCache[getAoTexPath()];
    textures.emplace_back(std::move(engine::Texture{ aoMapId, "texture_ao", getAoTexPath() }));

    heightMapId = engine::TextureManager::textureIDCache[getHeightTexPath()];
    textures.emplace_back(std::move(engine::Texture{ heightMapId, "texture_height", getHeightTexPath() }));

    emissiveMapId = engine::TextureManager::textureIDCache[getEmissiveTexPath()];
    textures.emplace_back(std::move(engine::Texture{ emissiveMapId, "texture_emissive", getEmissiveTexPath() }));


    if (diffuseMapId > 0) // BOFFFFF !!!!
    {
        //std::cout << "All textures loaded !!!!!" << std::endl;
        m_allTexturesLoaded = true;
    }
}

void engine::Material::setCubeMapTexs(const std::vector<std::string>& faces)
{
    m_cubemapTextures = faces;
}

