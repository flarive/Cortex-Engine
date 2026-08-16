#pragma once

#include "material.h"

namespace engine
{
	class PBRMaterial final : public Material
	{
	public:
		PBRMaterial(const Color& baseColorFactor);
		PBRMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor, float shininess = 0.0f);
		PBRMaterial(const Color& baseColorFactor, const std::string& diffuseTexPath, const std::string& normalTexPath = ""
			, const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = ""
			, const std::string& heightTexPath = "", const std::string& emissiveTexPath = "", float shininess = 0.0f);

		PBRMaterial(CombinedTexture combinedTextureType, const Color& baseColorFactor,
			const std::string& diffuseTexPath,
			const std::string& normalTexPath,
			const std::string& armOrRmTexPath,     // <-- packed AO/Roughness/Metallic or packed Roughness/Metallic
			const std::string& heightTexPath,
			const std::string& emissiveTexPath,
			float shininess = 0.0f);


		PBRMaterial(std::vector<Texture> _textures, float _shininess);

		MaterialType getTypeID() const override
		{
			return MaterialType::PBR;
		}
	};
}