#include "../../include/materials/pbr_material.h"


engine::PBRMaterial::PBRMaterial(const Color& ambientColor) : Material(ambientColor)
{
}

engine::PBRMaterial::PBRMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess) 
	: Material(ambientColor, diffuseColor, specularColor, shininess)
{
}

engine::PBRMaterial::PBRMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath,
	const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath,
	const std::string& heightTexPath, float shininess) : Material(ambientColor, diffuseTexPath, specularTexPath, normalTexPath, metallicTexPath, roughnessTexPath, aoTexPath)
{
}
