#include "../../include/materials/material.h"

engine::Material::Material()
{
}

engine::Material::Material(const engine::Color& ambientColor, const engine::Color& diffuseColor, float shininess)
	: m_ambientColor(ambientColor), m_diffuseColor(diffuseColor), m_shininess(shininess)
{
}

engine::Material::Material(const engine::Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& normalTexPath, float shininess)
	: m_ambientColor(ambientColor), m_diffuseTexPath(diffuseTexPath), m_specularTexPath(specularTexPath), m_normalTexPath(normalTexPath), m_shininess(shininess)
{
}

void engine::Material::setCubeMapTexs(std::vector<std::string> faces)
{
	m_cubemapTextures = faces;
}
