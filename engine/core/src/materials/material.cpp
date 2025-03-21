#include "../../include/materials/material.h"

#include <future>
#include <iostream>
#include <string>
#include <unordered_map>

//namespace {
//    // Global texture cache to prevent redundant loading
//    std::unordered_map<std::string, unsigned int> textureCache;
//}


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
unsigned int engine::Material::loadTextureAsync(const std::string& path, bool generateMipmaps, bool repeat, bool sRGB, bool gammaCorrection)
{
    if (path.empty()) return 0; // Avoid unnecessary loading

    // Check cache first
    //auto it = engine::TextureManager::textureCache.find(path);
    //if (it != engine::TextureManager::textureCache.end()) return it->second;

    // Load texture
    //unsigned int textureID = engine::Texture::loadTexture(path, repeat);


    std::future<std::tuple<unsigned char*, int, int, int>> ss = std::async(std::launch::async, &engine::Texture::loadTextureAsync, path, repeat);

    engine::TextureManager::textureCache[path] = std::move(ss); // Store in cache



    
    unsigned int textureID = 0;
    std::cout << "Loaded texture " << textureID << " - " << path << std::endl;
    return textureID;
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


//void engine::Material::loadTextures()
//{
//    textures.clear();  // Prevent duplicates
//    
//    unsigned int diffuseMapId = hasDiffuseMap() ? engine::Texture::loadTexture(getDiffuseTexPath(), true, false) : 0;
//    textures.emplace_back(std::move(engine::Texture{ diffuseMapId, "texture_diffuse", getDiffuseTexPath() }));
//
//    unsigned int specularMapId = hasSpecularMap() ? engine::Texture::loadTexture(getSpecularTexPath(), true, false) : 0;
//    textures.emplace_back(std::move(engine::Texture{ specularMapId, "texture_specular", getSpecularTexPath() }));
//
//    unsigned int normalMapId = hasNormalMap() ? engine::Texture::loadTexture(getNormalTexPath(), true, false) : 0;
//    textures.emplace_back(std::move(engine::Texture{ normalMapId, "texture_normal", getNormalTexPath() }));
//
//    unsigned int metallicMapId = hasMetallicMap() ? engine::Texture::loadTexture(getMetallicTexPath(), true, false) : 0;
//    textures.emplace_back(std::move(engine::Texture{ metallicMapId, "texture_metalness", getMetallicTexPath() }));
//
//    unsigned int roughnessMapId = hasRoughnessMap() ? engine::Texture::loadTexture(getRoughnessTexPath(), true, false) : 0;
//    textures.emplace_back(std::move(engine::Texture{ roughnessMapId, "texture_roughness", getRoughnessTexPath() }));
//
//    unsigned int aoMapId = hasAoMap() ? engine::Texture::loadTexture(getAoTexPath(), true, false) : 0;
//    textures.emplace_back(std::move(engine::Texture{ aoMapId, "texture_ao", getAoTexPath() }));
//
//    unsigned int heightMapId = hasHeightMap() ? engine::Texture::loadTexture(getHeightTexPath(), true, false) : 0;
//    textures.emplace_back(std::move(engine::Texture{ heightMapId, "texture_height", getHeightTexPath() }));
//}

//void engine::Material::loadTextures()
//{
//    textures.clear();
//    textures.reserve(7); // Preallocate memory
//
//    // Load textures asynchronously (Deferred OpenGL Processing)
//    {
//        std::lock_guard<std::mutex> lock(engine::Texture::textureCacheMutex);
//
//        if (hasDiffuseMap())   engine::Texture::textureCache[getDiffuseTexPath()] = std::async(std::launch::async, engine::Texture::loadTextureAsync, getDiffuseTexPath(), true);
//        if (hasSpecularMap())  engine::Texture::textureCache[getSpecularTexPath()] = std::async(std::launch::async, engine::Texture::loadTextureAsync, getSpecularTexPath(), true);
//        if (hasNormalMap())    engine::Texture::textureCache[getNormalTexPath()] = std::async(std::launch::async, engine::Texture::loadTextureAsync, getNormalTexPath(), true);
//        if (hasMetallicMap())  engine::Texture::textureCache[getMetallicTexPath()] = std::async(std::launch::async, engine::Texture::loadTextureAsync, getMetallicTexPath(), true);
//        if (hasRoughnessMap()) engine::Texture::textureCache[getRoughnessTexPath()] = std::async(std::launch::async, engine::Texture::loadTextureAsync, getRoughnessTexPath(), true);
//        if (hasAoMap())        engine::Texture::textureCache[getAoTexPath()] = std::async(std::launch::async, engine::Texture::loadTextureAsync, getAoTexPath(), true);
//        if (hasHeightMap())    engine::Texture::textureCache[getHeightTexPath()] = std::async(std::launch::async, engine::Texture::loadTextureAsync, getHeightTexPath(), true);
//    }
//
//    // Defer OpenGL calls to main thread
//    engine::Texture::enqueueTextureCreation(getDiffuseTexPath(), true);
//    engine::Texture::enqueueTextureCreation(getSpecularTexPath(), true);
//    engine::Texture::enqueueTextureCreation(getNormalTexPath(), true);
//    engine::Texture::enqueueTextureCreation(getMetallicTexPath(), true);
//    engine::Texture::enqueueTextureCreation(getRoughnessTexPath(), true);
//    engine::Texture::enqueueTextureCreation(getAoTexPath(), true);
//    engine::Texture::enqueueTextureCreation(getHeightTexPath(), true);
//}

void engine::Material::loadTextures()
{
    using namespace engine::TextureManager;  // Import namespace for convenience

    textures.clear();
    textures.reserve(7);

    // Load asynchronously
    loadTextureAsync(getDiffuseTexPath(), true);
    loadTextureAsync(getSpecularTexPath(), true);
    loadTextureAsync(getNormalTexPath(), true);
    loadTextureAsync(getMetallicTexPath(), true);
    loadTextureAsync(getRoughnessTexPath(), true);
    loadTextureAsync(getAoTexPath(), true);
    loadTextureAsync(getHeightTexPath(), true);

    // Queue OpenGL execution on main thread
    //unsigned int diffuseMapId = engine::Texture::enqueueTextureCreation(getDiffuseTexPath(), true);
    //engine::Texture::enqueueTextureCreation(getSpecularTexPath(), true);
    //engine::Texture::enqueueTextureCreation(getNormalTexPath(), true);
    //engine::Texture::enqueueTextureCreation(getMetallicTexPath(), true);
    //engine::Texture::enqueueTextureCreation(getRoughnessTexPath(), true);
    //engine::Texture::enqueueTextureCreation(getAoTexPath(), true);
    //engine::Texture::enqueueTextureCreation(getHeightTexPath(), true);


    unsigned int diffuseMapId = hasDiffuseMap() ? engine::Texture::enqueueTextureCreation(getDiffuseTexPath(), true) : 0;

    unsigned int diffuseMapId2 = engine::TextureManager::textureIDCache[getDiffuseTexPath()];

    if (engine::TextureManager::textureIDCache.size() > 0)
    {
        int q = 0;
    }


    textures.emplace_back(std::move(engine::Texture{ diffuseMapId2, "texture_diffuse", getDiffuseTexPath() }));
}




void engine::Material::setCubeMapTexs(const std::vector<std::string>& faces)
{
    m_cubemapTextures = faces;
}