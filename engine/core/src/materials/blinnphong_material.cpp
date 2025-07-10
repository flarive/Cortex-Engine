#include "../../include/materials/blinnphong_material.h"


engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor) 
	: Material(ambientColor)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess) 
	: Material(ambientColor, diffuseColor, specularColor, shininess)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, 
	const std::string& normalTexPath, float shininess) 
	: Material(ambientColor, diffuseTexPath, specularTexPath, normalTexPath, "", "", "")
{
}