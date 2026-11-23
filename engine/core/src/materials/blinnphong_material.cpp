#include "../../include/materials/blinnphong_material.h"

#include "../../include/managers/filesystem_manager.h"
#include "../../include/managers/texture_manager.h"

using fsm = engine::FileSystemManager;

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor) 
	: Material(MaterialType::blinnphong)
    , m_ambientColor(ambientColor)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess) 
	: Material(MaterialType::blinnphong)
    , m_ambientColor(ambientColor)
    , m_diffuseColor(diffuseColor)
    , m_specularColor(specularColor)
    , m_shininess(shininess)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, 
	const std::string& normalTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath, float shininess)
	: Material(MaterialType::blinnphong, diffuseTexPath, normalTexPath, heightTexPath, emissiveTexPath, opacityTexPath)
    , m_specularTexPath(fsm::getFullPath(specularTexPath))
    , m_ambientColor(ambientColor)
    , m_shininess(shininess)
{
}

bool engine::BlinnPhongMaterial::hasTextureMap()
{
    return hasDiffuseMap() || hasSpecularMap() || hasHeightMap() || hasEmissiveMap() || hasOpacityMap();
}

void engine::BlinnPhongMaterial::loadTextures()
{
    m_textures.clear();  // Prevent duplicates
    m_textures.reserve(6);
   
    // Load textures synchronously
    m_diffuseMapId = hasDiffuseMap() ? engine::TextureManager::loadTexture(m_diffuseTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    addTexture(std::make_shared<Texture>(m_diffuseMapId, "texture_diffuse", m_diffuseTexPath));

    m_specularMapId = hasSpecularMap() ? engine::TextureManager::loadTexture(m_specularTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    addTexture(std::make_shared<Texture>(m_specularMapId, "texture_specular", m_specularTexPath));

    m_normalMapId = hasNormalMap() ? engine::TextureManager::loadTexture(m_normalTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    addTexture(std::make_shared<Texture>(m_normalMapId, "texture_normal", m_normalTexPath));

    m_heightMapId = hasHeightMap() ? engine::TextureManager::loadTexture(m_heightTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    addTexture(std::make_shared<Texture>(m_heightMapId, "texture_height", m_heightTexPath));

    m_emissiveMapId = hasEmissiveMap() ? engine::TextureManager::loadTexture(m_emissiveTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    addTexture(std::make_shared<Texture>(m_emissiveMapId, "texture_emissive", m_emissiveTexPath));

    m_opacityMapId = hasOpacityMap() ? engine::TextureManager::loadTexture(m_opacityTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    addTexture(std::make_shared<Texture>(m_opacityMapId, "texture_opacity", m_opacityTexPath));

    m_allTexturesLoaded = true;
}

void engine::BlinnPhongMaterial::loadTexturesAsync(bool flipY, std::function<void(bool)> texturesLoadedCallback)
{
    m_textures.clear();
    m_textures.reserve(6);

    // Load textures asynchronously
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Diffuse, m_diffuseTexPath, flipY);
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Specular, m_specularTexPath, flipY);
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Normal, m_normalTexPath, flipY);
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Height, m_heightTexPath, flipY);
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Emissive, m_emissiveTexPath, flipY);
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Opacity, m_opacityTexPath, flipY);

    // Queue OpenGL execution on main thread
    m_diffuseMapId = hasDiffuseMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_diffuseTexPath) : 0;
    m_specularMapId = hasSpecularMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_specularTexPath) : 0;
    m_normalMapId = hasNormalMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_normalTexPath) : 0;
    m_heightMapId = hasHeightMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_heightTexPath) : 0;
    m_emissiveMapId = hasEmissiveMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_emissiveTexPath) : 0;
    m_opacityMapId = hasOpacityMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_opacityTexPath) : 0;
    


    // process queue
    engine::TextureManager::processLoadedTextures();


    // get loaded TextureID from queue
    m_diffuseMapId = engine::TextureManagerInternal::textureIDCache[m_diffuseTexPath];
    if (m_diffuseMapId > 0)
        addTexture(std::make_shared<Texture>(m_diffuseMapId, "texture_diffuse", m_diffuseTexPath));

    m_specularMapId = engine::TextureManagerInternal::textureIDCache[m_specularTexPath];
    if (m_specularMapId > 0)
        addTexture(std::make_shared<Texture>(m_specularMapId, "texture_specular", m_specularTexPath));

    m_normalMapId = engine::TextureManagerInternal::textureIDCache[m_normalTexPath];
    if (m_normalMapId > 0)
        addTexture(std::make_shared<Texture>(m_normalMapId, "texture_normal", m_normalTexPath));
    
    m_heightMapId = engine::TextureManagerInternal::textureIDCache[getHeightTexPath()];
    if (m_heightMapId > 0)
        addTexture(std::make_shared<Texture>(m_heightMapId, "texture_height", getHeightTexPath()));

    m_emissiveMapId = engine::TextureManagerInternal::textureIDCache[getEmissiveTexPath()];
    if (m_emissiveMapId > 0)
        addTexture(std::make_shared<Texture>(m_emissiveMapId, "texture_emissive", getEmissiveTexPath()));

    m_opacityMapId = engine::TextureManagerInternal::textureIDCache[getOpacityTexPath()];
    if (m_opacityMapId > 0)
        addTexture(std::make_shared<Texture>(m_opacityMapId, "texture_opacity", getOpacityTexPath()));

    int expectedCount = 0;

    // Count expected textures based on material type
    if (hasDiffuseMap())  expectedCount++;
    if (hasSpecularMap()) expectedCount++;
    if (hasNormalMap())   expectedCount++;
    if (hasHeightMap())   expectedCount++;
    if (hasEmissiveMap()) expectedCount++;
    if (hasOpacityMap())  expectedCount++;

    // Now compare expected vs loaded
    m_allTexturesLoaded = (m_textures.size() == expectedCount);

    if (texturesLoadedCallback)
        texturesLoadedCallback(m_allTexturesLoaded);
}
