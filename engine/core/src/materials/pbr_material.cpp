#include "../../include/materials/pbr_material.h"

engine::PBRMaterial::PBRMaterial(std::vector<Texture> _textures, float _shininess)
	: Material(std::move(_textures), _shininess)
{
}

engine::PBRMaterial::PBRMaterial(const Color& baseColorFactor)
	: Material(MaterialType::PBR, baseColorFactor)
{
}

engine::PBRMaterial::PBRMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess) 
	: Material(MaterialType::PBR, ambientColor, diffuseColor, specularColor, shininess)
{
}

engine::PBRMaterial::PBRMaterial(const Color& baseColorFactor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, float shininess)
	: Material(MaterialType::PBR, baseColorFactor, diffuseTexPath, "", normalTexPath, metallicTexPath, roughnessTexPath, aoTexPath, heightTexPath, emissiveTexPath, shininess)
{
}

/// <summary>
/// PBR material with MR, ARM combined textures
/// </summary>
engine::PBRMaterial::PBRMaterial(CombinedTexture combinedTextureType, const Color& baseColorFactor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& rmOrArmTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, float shininess)
	: Material(MaterialType::PBR, combinedTextureType, baseColorFactor, diffuseTexPath, "", normalTexPath, rmOrArmTexPath, heightTexPath, emissiveTexPath, shininess)
{
}