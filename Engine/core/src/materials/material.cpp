#include "../../include/materials/material.h"

engine::Material::Material(const engine::Color& ambientColor, const engine::Color& diffuseColor, float shininess)
	: m_ambientColor(ambientColor), m_diffuseColor(diffuseColor), m_shininess(shininess)
{
}

engine::Material::Material(const engine::Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath, const std::string& heightTexPath, float shininess)
	: m_ambientColor(ambientColor), m_diffuseTexPath(diffuseTexPath), m_specularTexPath(specularTexPath), m_normalTexPath(normalTexPath), m_metallicTexPath(metallicTexPath), m_roughnessTexPath(roughnessTexPath), m_aoTexPath(aoTexPath), m_heightTexPath(heightTexPath), m_shininess(shininess)
{
}

void engine::Material::setCubeMapTexs(const std::vector<std::string>& faces)
{
	m_cubemapTextures = faces;
}
