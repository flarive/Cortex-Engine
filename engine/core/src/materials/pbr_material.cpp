#include "../../include/materials/pbr_material.h"

#include "../../include/managers/filesystem_manager.h"
#include "../../include/managers/texture_manager.h"

using fsm = engine::FileSystemManager;

/// <summary>
/// PBR material with no textures, just a base color
/// </summary>
engine::PBRMaterial::PBRMaterial(const Color& baseColorFactor)
	: Material(), m_baseColorFactor(baseColorFactor)
{
}

/// <summary>
/// PBR material with no combined textures
/// </summary>
engine::PBRMaterial::PBRMaterial(const Color& baseColorFactor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath)
	: Material(diffuseTexPath, normalTexPath, heightTexPath, emissiveTexPath, opacityTexPath)
    , m_metallicTexPath(fsm::getFullPath(metallicTexPath))
    , m_roughnessTexPath(fsm::getFullPath(roughnessTexPath))
    , m_aoTexPath(fsm::getFullPath(aoTexPath))
    , m_baseColorFactor(baseColorFactor)
{
}

/// <summary>
/// PBR material with MR, ARM combined textures
/// </summary>
engine::PBRMaterial::PBRMaterial(CombinedTexture combinedTextureType, const Color& baseColorFactor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& rmOrArmTexPath, const std::string& aoTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath)
	: Material(combinedTextureType, diffuseTexPath, normalTexPath, heightTexPath, emissiveTexPath, opacityTexPath)
    , m_baseColorFactor(baseColorFactor)
{
    if (combinedTextureType == CombinedTexture::RM)
    {
        // packed Roughness/Metallic
        m_rmTexPath = rmOrArmTexPath;
        m_aoTexPath = aoTexPath;
    }
    else if (combinedTextureType == CombinedTexture::ARM)
    {
        // packed AO/Roughness/Metallic
        m_armTexPath = rmOrArmTexPath;
    }
}

bool engine::PBRMaterial::hasTextureMap()
{
	return hasDiffuseMap() || hasNormalMap() || hasMetallicMap() || hasRoughnessMap() || hasAoMap() || hasArmMap() || hasRmMap() || hasHeightMap() || hasEmissiveMap() || hasOpacityMap();
}

void engine::PBRMaterial::loadTextures()
{
    m_textures.clear();  // Prevent duplicates
    m_textures.reserve(8);

    // Load textures synchronously
    engine::TextureManager::loadTexture(m_diffuseTexPath, TextureSlot::Diffuse, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_normalTexPath, TextureSlot::Normal, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_metallicTexPath, TextureSlot::Metallic, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_roughnessTexPath, TextureSlot::Roughness, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_aoTexPath, TextureSlot::AO, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_heightTexPath, TextureSlot::Height, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_emissiveTexPath, TextureSlot::Emissive, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    engine::TextureManager::loadTexture(m_opacityTexPath, TextureSlot::Opacity, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture);
    
    m_allTexturesLoaded = true;
}

void engine::PBRMaterial::loadTexturesAsync(bool flipY, std::function<void(bool)> texturesLoadedCallback)
{
    m_textures.clear();
    m_textures.reserve(8);
    
    // Load textures asynchronously
    // request load textures async on another thread
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Diffuse, m_diffuseTexPath, flipY);
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Normal, m_normalTexPath, flipY);
        
    if (hasArmMap())
    {
        // ao + roughness + metallic
        engine::TextureManager::requestLoadTextureAsync(TextureSlot::ARM, m_armTexPath, flipY);
    }
    else if (hasRmMap())
    {
        // roughness + metallic
        engine::TextureManager::requestLoadTextureAsync(TextureSlot::RM, m_rmTexPath, flipY);
        engine::TextureManager::requestLoadTextureAsync(TextureSlot::AO, m_aoTexPath, flipY);
    }
    else
    {
        // no combined textures
        engine::TextureManager::requestLoadTextureAsync(TextureSlot::Metallic, m_metallicTexPath, flipY);
        engine::TextureManager::requestLoadTextureAsync(TextureSlot::Roughness, m_roughnessTexPath, flipY);
        engine::TextureManager::requestLoadTextureAsync(TextureSlot::AO, m_aoTexPath, flipY);
    }
        
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Height, m_heightTexPath, flipY);
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Emissive, m_emissiveTexPath, flipY);
    engine::TextureManager::requestLoadTextureAsync(TextureSlot::Opacity, m_opacityTexPath, flipY);



    // Queue OpenGL execution on main thread
    if (hasDiffuseMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_diffuseTexPath, TextureSlot::Diffuse);
    if (hasNormalMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_normalTexPath, TextureSlot::Normal);

    if (hasArmMap())
    {
        // ARM combined texture(ambient occlusion, roughness and metalness)
        engine::TextureManager::enqueueAsyncTextureCreation(m_armTexPath, TextureSlot::ARM);
    }
    else if (hasRmMap())
    {
        // RM combined texture(roughness and metalness)
        engine::TextureManager::enqueueAsyncTextureCreation(m_rmTexPath, TextureSlot::RM);
        if (hasAoMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_aoTexPath, TextureSlot::AO);
    }
    else
    {
        if (hasMetallicMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_metallicTexPath, TextureSlot::Metallic);
        if (hasRoughnessMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_roughnessTexPath, TextureSlot::Roughness);
        if (hasAoMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_aoTexPath, TextureSlot::AO);
    }
        
    if (hasHeightMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_heightTexPath, TextureSlot::Height);
    if (hasEmissiveMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_emissiveTexPath, TextureSlot::Emissive);
    if (hasOpacityMap()) engine::TextureManager::enqueueAsyncTextureCreation(m_opacityTexPath, TextureSlot::Opacity);

    // process queue
    engine::TextureManager::processLoadedTextures();

    
    // get loaded textures from cache
    addTexture(engine::TextureManager::getTextureFromCache(m_diffuseTexPath));
    addTexture(engine::TextureManager::getTextureFromCache(m_normalTexPath));


    if (hasArmMap())
    {
        // ao + roughness + metalness
        addTexture(engine::TextureManager::getTextureFromCache(m_armTexPath));
    }
    else if (hasRmMap())
    {
        // roughness + metalness
        addTexture(engine::TextureManager::getTextureFromCache(m_rmTexPath));
        
        // ao
        addTexture(engine::TextureManager::getTextureFromCache(m_aoTexPath));
    }
    else
    {
        addTexture(engine::TextureManager::getTextureFromCache(m_aoTexPath));
        addTexture(engine::TextureManager::getTextureFromCache(m_roughnessTexPath));
        addTexture(engine::TextureManager::getTextureFromCache(m_metallicTexPath));
    }

    addTexture(engine::TextureManager::getTextureFromCache(m_heightTexPath));
    addTexture(engine::TextureManager::getTextureFromCache(m_emissiveTexPath));
    addTexture(engine::TextureManager::getTextureFromCache(m_opacityTexPath));


    // Now compare expected vs loaded
    m_allTexturesLoaded = checkAllTexturesLoaded();

    if (texturesLoadedCallback)
        texturesLoadedCallback(m_allTexturesLoaded);
}

bool engine::PBRMaterial::checkAllTexturesLoaded() const
{
    int expectedCount = 0;

    // Count expected textures based on material type
    if (hasDiffuseMap())  expectedCount++;
    if (hasNormalMap())   expectedCount++;

    if (hasArmMap()) {
        expectedCount++;
    }
    else if (hasRmMap()) {
        expectedCount++;
        if (hasAoMap())       expectedCount++;
    }
    else {
        if (hasMetallicMap()) expectedCount++;
        if (hasRoughnessMap()) expectedCount++;
        if (hasAoMap())       expectedCount++;
    }
    if (hasHeightMap())   expectedCount++;
    if (hasEmissiveMap()) expectedCount++;
    if (hasOpacityMap()) expectedCount++;


    // Now compare expected vs loaded
    return m_textures.size() == expectedCount;
}
