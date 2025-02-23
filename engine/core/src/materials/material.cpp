#include "../../include/materials/material.h"

#include <iostream>

engine::Material::Material(const std::vector<Texture>& textures)
    : textures(textures)
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


void engine::Material::bind(Shader& shader, bool test) const
{
    unsigned int textureUnit = 0;

    for (const auto& texture : textures)
    {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, texture.id);

        // Pass the texture unit to the shader
        shader.setInt("material." + texture.type + "1", textureUnit);

        shader.setBool("material.has_" + texture.type + "_map", texture.id > 0);

        textureUnit++;
    }

    glActiveTexture(GL_TEXTURE0); // Reset active texture
}

void engine::Material::unbind() const
{
    for (size_t i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
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
}

void engine::Material::setCubeMapTexs(const std::vector<std::string>& faces)
{
    m_cubemapTextures = faces;
}