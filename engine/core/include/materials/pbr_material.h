#pragma once

#include "material.h"

#include "../cameras/camera.h"

namespace engine
{
	class PBRMaterial final : public Material
	{
	public:
		PBRMaterial(const Color& ambientColor);
		PBRMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess = 0.0f);
		PBRMaterial(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& normalTexPath = ""
			, const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = ""
			, const std::string& heightTexPath = "", float shininess = 0.0f);

		PBRMaterial(CombinedTexture combinedTextureType, const Color& ambientColor,
			const std::string& diffuseTexPath,
			const std::string& normalTexPath,
			const std::string& armOrRmTexPath,     // <-- packed AO/Roughness/Metallic or packed Roughness/Metallic
			const std::string& heightTexPath,
			float shininess = 0.0f);


		PBRMaterial(std::vector<Texture> _textures, float _shininess);

		MaterialType getTypeID() const override
		{
			return MaterialType::PBR;
		}
	};
}