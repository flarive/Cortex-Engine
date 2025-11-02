#include "../../include/materials/material.h"

#include <format>

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

bool engine::Material::bind(engine::Shader& shader) const
{
    unsigned int textureUnit = 0;
    bool success = true;

    for (const auto& texture : textures)
    {
        const std::string uniformName = std::format("material.{}", texture.type);
        const std::string hasMapName = std::format("material.has_{}_map", texture.type);

        if (texture.id > 0)
        {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texture.id);

            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
            {
                std::cerr << "OpenGL error while binding texture '" << texture.type
                    << "' to unit " << textureUnit << ": " << std::hex << error << std::endl;
                shader.setBool(hasMapName, false);
                success = false;
                continue;
            }

            shader.setInt(uniformName, textureUnit);
            shader.setBool(hasMapName, true);
            textureUnit++;
        }
        else
        {
            std::cerr << "Warning: Texture ID is 0 for '" << texture.type << "'. Texture might not be loaded correctly." << std::endl;
            shader.setBool(hasMapName, false);
            success = false;
        }
    }

    glActiveTexture(GL_TEXTURE0); // Reset active texture
    return success;
}

void engine::Material::unbind() const
{
    for (size_t i = 0; i < textures.size(); ++i)
    {
        if (textures[i].id > 0)
        {
            glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));
            glBindTexture(GL_TEXTURE_2D, 0);

            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
            {
                std::cerr << "OpenGL error while unbinding texture unit " << i << ": " << std::hex << error << std::endl;
            }
        }
    }

    glActiveTexture(GL_TEXTURE0); // Reset to default
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
    textures.reserve(8);

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
    if (diffuseMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ diffuseMapId, "texture_diffuse", getDiffuseTexPath() }));

    specularMapId = engine::TextureManager::textureIDCache[getSpecularTexPath()];
    if (specularMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ specularMapId, "texture_specular", getSpecularTexPath() }));

    normalMapId = engine::TextureManager::textureIDCache[getNormalTexPath()];
    if (normalMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ normalMapId, "texture_normal", getNormalTexPath() }));

    metallicMapId = engine::TextureManager::textureIDCache[getMetallicTexPath()];
    if (metallicMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ metallicMapId, "texture_metalness", getMetallicTexPath() }));

    roughnessMapId = engine::TextureManager::textureIDCache[getRoughnessTexPath()];
    if (roughnessMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ roughnessMapId, "texture_roughness", getRoughnessTexPath() }));

    aoMapId = engine::TextureManager::textureIDCache[getAoTexPath()];
    if (aoMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ aoMapId, "texture_ao", getAoTexPath() }));

    heightMapId = engine::TextureManager::textureIDCache[getHeightTexPath()];
    if (heightMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ heightMapId, "texture_height", getHeightTexPath() }));

    emissiveMapId = engine::TextureManager::textureIDCache[getEmissiveTexPath()];
    if (emissiveMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ emissiveMapId, "texture_emissive", getEmissiveTexPath() }));


    if (diffuseMapId > 0)
    {
        //std::cout << "All textures loaded !!!!!" << std::endl;
        m_allTexturesLoaded = true;
    }
}

void engine::Material::setCubeMapTexs(const std::vector<std::string>& faces)
{
    m_cubemapTextures = faces;
}

