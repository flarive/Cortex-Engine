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

engine::Material::Material(CombinedTexture mode, const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& normalTexPath, const std::string& rmOrArmTexPath, const std::string& heightTexPath)
    : m_ambientColor(ambientColor), m_diffuseTexPath(diffuseTexPath), m_specularTexPath(specularTexPath), m_normalTexPath(normalTexPath), m_heightTexPath(heightTexPath), m_shininess(0.0f)
{
    if (mode == CombinedTexture::RM)
    {
        // packed Roughness/Metallic
        m_rmTexPath = rmOrArmTexPath;
    }
    else if (mode == CombinedTexture::ARM)
    {
        // packed AO/Roughness/Metallic
        m_armTexPath = rmOrArmTexPath;
    }
}

bool engine::Material::bind(engine::Shader& shader, int baseUnit) const
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
    shader.use();

    int unit = baseUnit;
    bool success = true;

	// cleanup any previous bindings for this shader's material uniforms
	if (shader.getShaderType() == ShaderType::PBR || shader.getShaderType() == ShaderType::PBRTessellation)
    {
        shader.setBool("material.has_texture_diffuse_map", false);
        shader.setBool("material.has_texture_normal_map", false);
        shader.setBool("material.has_texture_metalness_map", false);
        shader.setBool("material.has_texture_roughness_map", false);
        shader.setBool("material.has_texture_ao_map", false);
        shader.setBool("materialHeight.has_texture_height_map", false);
    }
    else
    {
        shader.setBool("material.has_texture_diffuse_map", false);
        shader.setBool("material.has_texture_specular_map", false);
        shader.setBool("material.has_texture_normal_map", false);
        shader.setBool("materialHeight.has_texture_height_map", false);
    }

    for (const auto& tex : textures)
    {
        // Example: tex.type could be "albedo", "normal", "metallic", ...
        std::string uniformName = std::format("material.{}", tex.type);
        std::string hasMapName = std::format("material.has_{}_map", tex.type);

        if (tex.type == "texture_height")
        {
            uniformName = std::format("materialHeight.{}", tex.type);
            hasMapName = std::format("materialHeight.has_{}_map", tex.type);
        }

        // If no texture, mark false and continue (don’t change unit).
        if (tex.id == 0)
        {
            //glActiveTexture(GL_TEXTURE0 + unit);
            //glBindTexture(GL_TEXTURE_2D, 0);

            //shader.setInt(uniformName, unit);
            shader.setBool(hasMapName, false);
            continue;
        }

        // Ensure we have room on the device
        if (unit >= m_maxFragUnits)
        {
            std::cerr << "[Material::bind] ERROR: Texture unit " << unit << " exceeds GL_MAX_TEXTURE_IMAGE_UNITS=" << m_maxFragUnits << " for uniform '" << uniformName << "'\n";
            shader.setBool(hasMapName, false);
            success = false;
            break; // or return false;
        }

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, tex.id);

#ifdef DEBUG
        // Keep this in debug only; glGetError is expensive
        if (GLenum error = glGetError(); error != GL_NO_ERROR)
        {
            std::cerr << std::hex
                << "[Material::bind] GL error 0x" << error << " binding '" << tex.type << "' to unit " << unit << " (target=" << target << ")\n" << std::dec;
            shader.setBool(hasMapName, false);
            success = false;
            // Continue to bind others, or break if you prefer fail-fast.
            ++unit;
            continue;
        }
#endif

        // Assign the sampler to the unit and flag presence
        shader.setInt(uniformName, unit);
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

    MaterialType type = getTypeID();

    // Load textures asynchronously
    if (type == MaterialType::PBR)
    {
        unsigned int diffuseMapId = hasDiffuseMap() ? engine::Texture::loadTexture(m_diffuseTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ diffuseMapId, "texture_diffuse", m_diffuseTexPath }));

        unsigned int normalMapId = hasNormalMap() ? engine::Texture::loadTexture(m_normalTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ normalMapId, "texture_normal", m_normalTexPath }));

        unsigned int metallicMapId = hasMetallicMap() ? engine::Texture::loadTexture(m_metallicTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ metallicMapId, "texture_metalness", m_metallicTexPath }));

        unsigned int roughnessMapId = hasRoughnessMap() ? engine::Texture::loadTexture(m_roughnessTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ roughnessMapId, "texture_roughness", m_roughnessTexPath }));

        unsigned int aoMapId = hasAoMap() ? engine::Texture::loadTexture(m_aoTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ aoMapId, "texture_ao", m_aoTexPath }));

        unsigned int heightMapId = hasHeightMap() ? engine::Texture::loadTexture(m_heightTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ heightMapId, "texture_height", m_heightTexPath }));

        unsigned int emissiveMapId = hasEmissiveMap() ? engine::Texture::loadTexture(m_emissiveTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ emissiveMapId, "texture_emissive", m_emissiveTexPath }));

        if (!m_armTexPath.empty())
        {
            int i = 0;
        }
    }
    else
    {
        // BlinnPhong, Phong...
        unsigned int diffuseMapId = hasDiffuseMap() ? engine::Texture::loadTexture(m_diffuseTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ diffuseMapId, "texture_diffuse", m_diffuseTexPath }));

        unsigned int specularMapId = hasSpecularMap() ? engine::Texture::loadTexture(m_specularTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ specularMapId, "texture_specular", m_specularTexPath }));

        unsigned int normalMapId = hasNormalMap() ? engine::Texture::loadTexture(m_normalTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ normalMapId, "texture_normal", m_normalTexPath }));

        unsigned int heightMapId = hasHeightMap() ? engine::Texture::loadTexture(m_heightTexPath, TextureFlag_GenerateMipmaps | TextureFlag_RepeatTexture) : 0;
        textures.emplace_back(std::move(engine::Texture{ heightMapId, "texture_height", m_heightTexPath }));
    }
}

void engine::Material::loadTexturesAsync(std::function<void(bool)> texturesLoadedCallback)
{
    textures.clear();
    
    unsigned int diffuseMapId{};
    unsigned int specularMapId{};
    unsigned int normalMapId{};
    unsigned int metallicMapId{};
    unsigned int roughnessMapId{};
    unsigned int aoMapId{};
    //unsigned int heightMapId{};
    unsigned int emissiveMapId{};


    unsigned int armMapId{}; // packed AO/Roughness/Metallic
    unsigned int rmMapId{}; // packed Roughness/Metallic


    MaterialType type = getTypeID();

    // Load textures asynchronously
    if (type == MaterialType::PBR)
    {
        // PBR
        textures.reserve(7);
        
        // request load textures async on another thread
        engine::Texture::requestLoadTextureAsync(m_diffuseTexPath);
        engine::Texture::requestLoadTextureAsync(m_normalTexPath);
        
        if (hasArmMap())
        {
            engine::Texture::requestLoadTextureAsync(m_armTexPath);
        }
        else if (hasRmMap())
        {
            engine::Texture::requestLoadTextureAsync(m_rmTexPath);
        }
        else
        {
            engine::Texture::requestLoadTextureAsync(m_metallicTexPath);
            engine::Texture::requestLoadTextureAsync(m_roughnessTexPath);
            engine::Texture::requestLoadTextureAsync(m_aoTexPath);
        }
        
        engine::Texture::requestLoadTextureAsync(m_heightTexPath);
        engine::Texture::requestLoadTextureAsync(m_emissiveTexPath);

        // Queue OpenGL execution on main thread
        diffuseMapId = hasDiffuseMap() ? engine::Texture::enqueueAsyncTextureCreation(m_diffuseTexPath) : 0;
        normalMapId = hasNormalMap() ? engine::Texture::enqueueAsyncTextureCreation(m_normalTexPath) : 0;

        if (hasArmMap())
        {
            // ARM combined texture(ambient occlusion, roughness and metalness)
            armMapId = engine::Texture::enqueueAsyncTextureCreation(m_armTexPath);

            aoMapId = 0;
            roughnessMapId = 0;
            metallicMapId = 0;
        }
        else if (hasRmMap())
        {
            // RM combined texture(roughness and metalness)
            rmMapId = engine::Texture::enqueueAsyncTextureCreation(m_rmTexPath);

            roughnessMapId = 0;
            metallicMapId = 0;
        }
        else
        {
            metallicMapId = hasMetallicMap() ? engine::Texture::enqueueAsyncTextureCreation(m_metallicTexPath) : 0;
            roughnessMapId = hasRoughnessMap() ? engine::Texture::enqueueAsyncTextureCreation(m_roughnessTexPath) : 0;
            aoMapId = hasAoMap() ? engine::Texture::enqueueAsyncTextureCreation(m_aoTexPath) : 0;
        }
        
        heightMapId = hasHeightMap() ? engine::Texture::enqueueAsyncTextureCreation(m_heightTexPath) : 0;
        emissiveMapId = hasEmissiveMap() ? engine::Texture::enqueueAsyncTextureCreation(m_emissiveTexPath) : 0;
    }
    else
    {
        // BlinnPhong, Blinn...
        textures.reserve(4);
        
        engine::Texture::requestLoadTextureAsync(m_diffuseTexPath);
        engine::Texture::requestLoadTextureAsync(m_specularTexPath);
        engine::Texture::requestLoadTextureAsync(m_normalTexPath);
        engine::Texture::requestLoadTextureAsync(m_heightTexPath);

        // Queue OpenGL execution on main thread
        diffuseMapId = hasDiffuseMap() ? engine::Texture::enqueueAsyncTextureCreation(m_diffuseTexPath) : 0;
        specularMapId = hasSpecularMap() ? engine::Texture::enqueueAsyncTextureCreation(m_specularTexPath) : 0;
        normalMapId = hasNormalMap() ? engine::Texture::enqueueAsyncTextureCreation(m_normalTexPath) : 0;
        heightMapId = hasHeightMap() ? engine::Texture::enqueueAsyncTextureCreation(m_heightTexPath) : 0;
    }


    // process queue
    engine::Texture::processLoadedTextures();

    
    // get loaded TextureID from queue
    diffuseMapId = engine::TextureManager::textureIDCache[m_diffuseTexPath];
    if (diffuseMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ diffuseMapId, "texture_diffuse", m_diffuseTexPath }));

    specularMapId = engine::TextureManager::textureIDCache[m_specularTexPath];
	if (specularMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ specularMapId, "texture_specular", m_specularTexPath }));

    normalMapId = engine::TextureManager::textureIDCache[m_normalTexPath];
    if (normalMapId > 0)
	    textures.emplace_back(std::move(engine::Texture{ normalMapId, "texture_normal", m_normalTexPath }));

    if (hasArmMap())
    {
        armMapId = engine::TextureManager::textureIDCache[m_armTexPath];
        if (armMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ armMapId, "texture_arm", m_armTexPath }));
    }
    else if (hasRmMap())
    {
        rmMapId = engine::TextureManager::textureIDCache[m_rmTexPath];
        if (rmMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ rmMapId, "texture_rm", m_rmTexPath }));
    }
    else
    {
        aoMapId = engine::TextureManager::textureIDCache[m_aoTexPath];
        if (aoMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ aoMapId, "texture_ao", m_aoTexPath }));

        roughnessMapId = engine::TextureManager::textureIDCache[m_roughnessTexPath];
        if (roughnessMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ roughnessMapId, "texture_roughness", m_roughnessTexPath }));

        metallicMapId = engine::TextureManager::textureIDCache[m_metallicTexPath];
        if (metallicMapId > 0)
            textures.emplace_back(std::move(engine::Texture{ metallicMapId, "texture_metalness", m_metallicTexPath }));
    }

    heightMapId = engine::TextureManager::textureIDCache[getHeightTexPath()];
    if (heightMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ heightMapId, "texture_height", getHeightTexPath() }));

    emissiveMapId = engine::TextureManager::textureIDCache[getEmissiveTexPath()];
    if (emissiveMapId > 0)
        textures.emplace_back(std::move(engine::Texture{ emissiveMapId, "texture_emissive", getEmissiveTexPath() }));


    if (diffuseMapId > 0) // BOFFFFF !!!!
    {
        // use textures.size() instead
        m_allTexturesLoaded = true;
    }

    if (texturesLoadedCallback)
        texturesLoadedCallback(m_allTexturesLoaded);
}

void engine::Material::setCubeMapTexs(const std::vector<std::string>& faces)
{
    m_cubemapTextures = faces;
}

const int engine::Material::getTextureHeightUnit() const
{
    return heightMapId;
}



