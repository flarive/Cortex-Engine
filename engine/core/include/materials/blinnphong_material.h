#pragma once

#include "material.h"

namespace engine
{
	/// <summary>
	/// Blinn Phong material (legacy)
	/// https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
	/// </summary>
	class BlinnPhongMaterial final : public Material
	{
	public:
		BlinnPhongMaterial(const Color& ambientColor);
		BlinnPhongMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess = 1.0f);
		BlinnPhongMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", const std::string& heightTexPath = "", const std::string& emissiveTexPath = "", const std::string& opacityTexPath = "", float shininess = 1.0f);
		BlinnPhongMaterial(std::vector<Texture> _textures, float _shininess);

		MaterialType getTypeID() const override
		{
			return MaterialType::blinnphong;
		}
	};
}