#include "../../include/materials/blinnphong_material.h"


engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor) : BaseMaterial(ambientColor)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const Color& diffuseColor, float shininess) : BaseMaterial(ambientColor, diffuseColor, shininess)
{
}

engine::BlinnPhongMaterial::BlinnPhongMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, 
	const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath, 
	const std::string& heightTexPath, float shininess) : BaseMaterial(ambientColor, diffuseTexPath, specularTexPath, normalTexPath, metallicTexPath, roughnessTexPath, aoTexPath)
{
}