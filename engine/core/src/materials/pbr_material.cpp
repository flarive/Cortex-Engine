#include "../../include/materials/pbr_material.h"

engine::PBRMaterial::PBRMaterial(std::vector<Texture> _textures, float _shininess)
	: Material(std::move(_textures), _shininess)
{}

engine::PBRMaterial::PBRMaterial(const Color& ambientColor) : Material(ambientColor)
{
}

engine::PBRMaterial::PBRMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess) 
	: Material(ambientColor, diffuseColor, specularColor, shininess)
{
}

engine::PBRMaterial::PBRMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath, const std::string& heightTexPath, float shininess)
	: Material(ambientColor, diffuseTexPath, "", normalTexPath, metallicTexPath, roughnessTexPath, aoTexPath, heightTexPath)
{
}

/// <summary>
/// PBR material with MR, ARM combined textures
/// </summary>
engine::PBRMaterial::PBRMaterial(CombinedTexture combinedTextureType, const Color& ambientColor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& rmOrArmTexPath, const std::string& heightTexPath, float shininess)
	: Material(combinedTextureType, ambientColor, diffuseTexPath, "", normalTexPath, rmOrArmTexPath, heightTexPath)
{
}