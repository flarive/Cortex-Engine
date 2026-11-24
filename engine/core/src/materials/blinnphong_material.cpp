#include "../../include/materials/blinnphong_material.h"

#include "../../include/managers/filesystem_manager.h"
#include "../../include/managers/texture_manager.h"

using fsm = engine::FileSystemManager;

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor) 
	: Material()
    , m_ambientColor(ambientColor)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess) 
	: Material()
    , m_ambientColor(ambientColor)
    , m_diffuseColor(diffuseColor)
    , m_specularColor(specularColor)
    , m_shininess(shininess)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, 
	const std::string& normalTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath, float shininess)
	: Material(diffuseTexPath, normalTexPath, heightTexPath, emissiveTexPath, opacityTexPath)
    , m_specularTexPath(fsm::getFullPath(specularTexPath))
    , m_ambientColor(ambientColor)
    , m_shininess(shininess)
{
}

bool engine::BlinnPhongMaterial::hasTextureMap()
{
    return hasDiffuseMap() || hasSpecularMap() || hasNormalMap() || hasHeightMap() || hasEmissiveMap() || hasOpacityMap();
}

void engine::BlinnPhongMaterial::loadTextures()
{
    m_textures.clear();  // Prevent duplicates
    m_textures.reserve(6);
   
    // Load textures synchronously
    engine::TextureManager::loadTexture(m_diffuseTexPath, TextureSlot::Diffuse, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_specularTexPath, TextureSlot::Specular, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_normalTexPath, TextureSlot::Normal, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_heightTexPath, TextureSlot::Height, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_emissiveTexPath, TextureSlot::Emissive, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_opacityTexPath, TextureSlot::Opacity, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    
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
    if (hasDiffuseMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_diffuseTexPath, TextureSlot::Diffuse);
    if (hasSpecularMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_specularTexPath, TextureSlot::Specular);
    if (hasNormalMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_normalTexPath, TextureSlot::Normal);
    if (hasHeightMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_heightTexPath, TextureSlot::Height);
    if (hasEmissiveMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_emissiveTexPath, TextureSlot::Emissive);
    if (hasOpacityMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_opacityTexPath, TextureSlot::Opacity);
    


    // process queue
    engine::TextureManager::processLoadedTextures();


    // get loaded textures from cache
    addTexture(engine::TextureManager::getTextureFromCache(m_diffuseTexPath));
    addTexture(engine::TextureManager::getTextureFromCache(m_specularTexPath));
    addTexture(engine::TextureManager::getTextureFromCache(m_normalTexPath));
    addTexture(engine::TextureManager::getTextureFromCache(m_heightTexPath));
    addTexture(engine::TextureManager::getTextureFromCache(m_emissiveTexPath));
    addTexture(engine::TextureManager::getTextureFromCache(m_opacityTexPath));


    // Now compare expected vs loaded
    m_allTexturesLoaded = checkAllTexturesLoaded();

    if (texturesLoadedCallback)
        texturesLoadedCallback(m_allTexturesLoaded);
}

bool engine::BlinnPhongMaterial::checkAllTexturesLoaded() const
{
    int expectedCount = 0;

    // Count expected textures based on material type
    if (hasDiffuseMap())  expectedCount++;
    if (hasSpecularMap()) expectedCount++;
    if (hasNormalMap())   expectedCount++;
    if (hasHeightMap())   expectedCount++;
    if (hasEmissiveMap()) expectedCount++;
    if (hasOpacityMap())  expectedCount++;

    // Now compare expected vs loaded
    return m_textures.size() == expectedCount;
}
