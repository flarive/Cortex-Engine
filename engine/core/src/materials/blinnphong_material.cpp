#include "../../include/materials/blinnphong_material.h"

engine::BlinnPhongMaterial::BlinnPhongMaterial(std::vector<Texture> _textures, float _shininess)
	: Material(std::move(_textures), _shininess)
{}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor) 
	: Material(MaterialType::blinnphong, ambientColor)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess) 
	: Material(MaterialType::blinnphong, ambientColor, diffuseColor, specularColor, shininess)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, 
	const std::string& normalTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, float shininess)
	: Material(MaterialType::blinnphong, ambientColor, diffuseTexPath, specularTexPath, normalTexPath, "", "", "", heightTexPath, emissiveTexPath, shininess)
{
}