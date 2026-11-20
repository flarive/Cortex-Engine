#include "../../include/materials/pbr_material.h"

#include "../../include/managers/filesystem_manager.h"
#include "../../include/managers/texture_manager.h"

using fsm = engine::FileSystemManager;

engine::PBRMaterial::PBRMaterial(std::vector<Texture> _textures)
	: Material(std::move(_textures))
{
    for (const Texture& tex : textures)
    {
        if (tex.type == "texture_metalness")
            m_metallicTexPath = tex.path;
        else if (tex.type == "texture_roughness")
            m_roughnessTexPath = tex.path;
        else if (tex.type == "texture_ao")
            m_aoTexPath = tex.path;
    }
}

/// <summary>
/// PBR material with no textures, just a base color
/// </summary>
engine::PBRMaterial::PBRMaterial(const Color& baseColorFactor)
	: Material(MaterialType::PBR, baseColorFactor)
{
    m_baseColorFactor = baseColorFactor;
    m_ambientColor = Color(1.0f);
}

/// <summary>
/// PBR material with no combined textures
/// </summary>
engine::PBRMaterial::PBRMaterial(const Color& baseColorFactor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath)
	: Material(MaterialType::PBR, baseColorFactor, diffuseTexPath, normalTexPath, heightTexPath, emissiveTexPath, opacityTexPath)
    , m_metallicTexPath(fsm::getFullPath(metallicTexPath))
    , m_roughnessTexPath(fsm::getFullPath(roughnessTexPath))
    , m_aoTexPath(fsm::getFullPath(aoTexPath))
{
    m_baseColorFactor = baseColorFactor;
    m_ambientColor = Color(1.0f);
}

/// <summary>
/// PBR material with MR, ARM combined textures
/// </summary>
engine::PBRMaterial::PBRMaterial(CombinedTexture combinedTextureType, const Color& baseColorFactor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& rmOrArmTexPath, const std::string& aoTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath)
	: Material(MaterialType::PBR, combinedTextureType, baseColorFactor, diffuseTexPath, normalTexPath, heightTexPath, emissiveTexPath, opacityTexPath)
{
    m_baseColorFactor = baseColorFactor;
    m_ambientColor = Color(1.0f);

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
    textures.clear();  // Prevent duplicates

    // Load textures synchronously
    m_diffuseMapId = hasDiffuseMap() ? engine::TextureManager::loadTexture(m_diffuseTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    textures.emplace_back(std::move(engine::Texture{ m_diffuseMapId, "texture_diffuse", m_diffuseTexPath }));

    m_normalMapId = hasNormalMap() ? engine::TextureManager::loadTexture(m_normalTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    textures.emplace_back(std::move(engine::Texture{ m_normalMapId, "texture_normal", m_normalTexPath }));

    m_metallicMapId = hasMetallicMap() ? engine::TextureManager::loadTexture(m_metallicTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    textures.emplace_back(std::move(engine::Texture{ m_metallicMapId, "texture_metalness", m_metallicTexPath }));

    m_roughnessMapId = hasRoughnessMap() ? engine::TextureManager::loadTexture(m_roughnessTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    textures.emplace_back(std::move(engine::Texture{ m_roughnessMapId, "texture_roughness", m_roughnessTexPath }));

    m_aoMapId = hasAoMap() ? engine::TextureManager::loadTexture(m_aoTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    textures.emplace_back(std::move(engine::Texture{ m_aoMapId, "texture_ao", m_aoTexPath }));

    m_heightMapId = hasHeightMap() ? engine::TextureManager::loadTexture(m_heightTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    textures.emplace_back(std::move(engine::Texture{ m_heightMapId, "texture_height", m_heightTexPath }));

    m_emissiveMapId = hasEmissiveMap() ? engine::TextureManager::loadTexture(m_emissiveTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    textures.emplace_back(std::move(engine::Texture{ m_emissiveMapId, "texture_emissive", m_emissiveTexPath }));

    m_opacityMapId = hasOpacityMap() ? engine::TextureManager::loadTexture(m_opacityTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
    textures.emplace_back(std::move(engine::Texture{ m_opacityMapId, "texture_opacity", m_opacityTexPath }));
    
    m_allTexturesLoaded = true;
}

void engine::PBRMaterial::loadTexturesAsync(bool flipY, std::function<void(bool)> texturesLoadedCallback)
{
    textures.clear();
    textures.reserve(8);
    
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
    m_diffuseMapId = hasDiffuseMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_diffuseTexPath) : 0;
    m_normalMapId = hasNormalMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_normalTexPath) : 0;

    if (hasArmMap())
    {
        // ARM combined texture(ambient occlusion, roughness and metalness)
        m_armMapId = engine::TextureManager::enqueueAsyncTextureCreation(m_armTexPath);

        m_aoMapId = 0;
        m_roughnessMapId = 0;
        m_metallicMapId = 0;
    }
    else if (hasRmMap())
    {
        // RM combined texture(roughness and metalness)
        m_rmMapId = engine::TextureManager::enqueueAsyncTextureCreation(m_rmTexPath);
        m_aoMapId = hasAoMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_aoTexPath) : 0;

        m_roughnessMapId = 0;
        m_metallicMapId = 0;
    }
    else
    {
        m_metallicMapId = hasMetallicMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_metallicTexPath) : 0;
        m_roughnessMapId = hasRoughnessMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_roughnessTexPath) : 0;
        m_aoMapId = hasAoMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_aoTexPath) : 0;
    }
        
    m_heightMapId = hasHeightMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_heightTexPath) : 0;
    m_emissiveMapId = hasEmissiveMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_emissiveTexPath) : 0;
    m_opacityMapId = hasOpacityMap() ? engine::TextureManager::enqueueAsyncTextureCreation(m_opacityTexPath) : 0;

    // process queue
    engine::TextureManager::processLoadedTextures();

    
    // get loaded TextureID from queue
    m_diffuseMapId = engine::TextureManagerInternal::textureIDCache[m_diffuseTexPath];
    if (m_diffuseMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ m_diffuseMapId, "texture_diffuse", m_diffuseTexPath }));

    m_normalMapId = engine::TextureManagerInternal::textureIDCache[m_normalTexPath];
    if (m_normalMapId > 0)
	    textures.emplace_back(std::move(engine::Texture{ m_normalMapId, "texture_normal", m_normalTexPath }));

    if (hasArmMap())
    {
        // ao + roughness + metalness
        m_armMapId = engine::TextureManagerInternal::textureIDCache[m_armTexPath];
        if (m_armMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ m_armMapId, "texture_arm", m_armTexPath }));
    }
    else if (hasRmMap())
    {
        // roughness + metalness
        m_rmMapId = engine::TextureManagerInternal::textureIDCache[m_rmTexPath];
        if (m_rmMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ m_rmMapId, "texture_rm", m_rmTexPath }));

        // ao
        m_aoMapId = engine::TextureManagerInternal::textureIDCache[m_aoTexPath];
        if (m_aoMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ m_aoMapId, "texture_ao", m_aoTexPath }));
    }
    else
    {
        m_aoMapId = engine::TextureManagerInternal::textureIDCache[m_aoTexPath];
        if (m_aoMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ m_aoMapId, "texture_ao", m_aoTexPath }));

        m_roughnessMapId = engine::TextureManagerInternal::textureIDCache[m_roughnessTexPath];
        if (m_roughnessMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ m_roughnessMapId, "texture_roughness", m_roughnessTexPath }));

        m_metallicMapId = engine::TextureManagerInternal::textureIDCache[m_metallicTexPath];
        if (m_metallicMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ m_metallicMapId, "texture_metalness", m_metallicTexPath }));
    }

    m_heightMapId = engine::TextureManagerInternal::textureIDCache[getHeightTexPath()];
    if (m_heightMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ m_heightMapId, "texture_height", getHeightTexPath() }));

    m_emissiveMapId = engine::TextureManagerInternal::textureIDCache[getEmissiveTexPath()];
    if (m_emissiveMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ m_emissiveMapId, "texture_emissive", getEmissiveTexPath() }));

    m_opacityMapId = engine::TextureManagerInternal::textureIDCache[getOpacityTexPath()];
    if (m_opacityMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ m_opacityMapId, "texture_opacity", getOpacityTexPath() }));

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
    m_allTexturesLoaded = (textures.size() == expectedCount);

    if (texturesLoadedCallback)
        texturesLoadedCallback(m_allTexturesLoaded);
}
