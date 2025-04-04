#include "../../include/materials/material.h"

#include <future>
#include <string>
#include <unordered_map>

engine::Material::Material(const std::vector<Texture>& textures)
    : textures(textures)
{
}

engine::Material::Material(const engine::Color& ambientColor)
    : m_ambientColor(ambientColor), m_diffuseTexPath(""), m_specularTexPath(""), m_normalTexPath(""), m_metallicTexPath(""), m_roughnessTexPath(""), m_aoTexPath(""), m_heightTexPath(""), m_shininess(0.0f)
{
}

engine::Material::Material(const engine::Color& ambientColor, const engine::Color& diffuseColor, float shininess)
    : m_ambientColor(ambientColor), m_diffuseColor(diffuseColor), m_shininess(shininess)
{
}

engine::Material::Material(const engine::Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath, const std::string& heightTexPath, float shininess)
    : m_ambientColor(ambientColor), m_diffuseTexPath(diffuseTexPath), m_specularTexPath(specularTexPath), m_normalTexPath(normalTexPath), m_metallicTexPath(metallicTexPath), m_roughnessTexPath(roughnessTexPath), m_aoTexPath(aoTexPath), m_heightTexPath(heightTexPath), m_shininess(shininess)
{
}

// Helper function to load a texture asynchronously
void engine::Material::loadTextureAsync(const std::string& path)
{
    if (path.empty()) return; // Avoid unnecessary loading

    // Check cache first
    auto it = engine::TextureManager::textureCache.find(path);
    if (it != engine::TextureManager::textureCache.end())
    {
        return;
    }
    
    // Load texture
    auto texture = std::async(std::launch::async, &engine::Texture::loadTextureAsyncInternal, path);
    engine::TextureManager::textureCache[path] = std::move(texture); // Store in cache
    
    return;
}



void engine::Material::bind(Shader& shader) const
{
    unsigned int textureUnit = 0;

    for (const auto& texture : textures)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, texture.id);

        // Pass the texture unit to the shader
        shader.setInt("material." + texture.type, textureUnit);
        shader.setBool("material.has_" + texture.type + "_map", texture.id > 0);

        textureUnit++;
    }

    glActiveTexture(GL_TEXTURE0); // Reset active texture
}

void engine::Material::unbind() const
{
    for (size_t i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + (GLenum)i);
        glBindTexture(GL_TEXTURE_2D, 0);
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
    textures.reserve(7);

    // Load textures asynchronously
    loadTextureAsync(getDiffuseTexPath());
    loadTextureAsync(getSpecularTexPath());
    loadTextureAsync(getNormalTexPath());
    loadTextureAsync(getMetallicTexPath());
    loadTextureAsync(getRoughnessTexPath());
    loadTextureAsync(getAoTexPath());
    loadTextureAsync(getHeightTexPath());
    loadTextureAsync(getEmissiveTexPath());

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

    heightMapId = engine::TextureManager::textureIDCache[getHeightTexPath()];
    textures.emplace_back(std::move(engine::Texture{ heightMapId, "texture_height", getHeightTexPath() }));

    emissiveMapId = engine::TextureManager::textureIDCache[getEmissiveTexPath()];
    textures.emplace_back(std::move(engine::Texture{ emissiveMapId, "texture_emissive", getEmissiveTexPath() }));
}

void engine::Material::setCubeMapTexs(const std::vector<std::string>& faces)
{
    m_cubemapTextures = faces;
}