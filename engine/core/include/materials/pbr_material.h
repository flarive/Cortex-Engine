#pragma once

#include "material.h"

namespace engine
{
	/// <summary>
	/// Physically Based Rendering material
	/// https://en.wikipedia.org/wiki/Physically_based_rendering
	/// </summary>
	class PBRMaterial final : public Material
	{
	public:
		PBRMaterial(const Color& baseColorFactor);
		PBRMaterial(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor);
		PBRMaterial(const Color& baseColorFactor, const std::string& diffuseTexPath, const std::string& normalTexPath = ""
			, const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = ""
			, const std::string& heightTexPath = "", const std::string& emissiveTexPath = "", const std::string& opacityTexPath = "");

		PBRMaterial(CombinedTexture combinedTextureType, const Color& baseColorFactor,
			const std::string& diffuseTexPath,
			const std::string& normalTexPath,
			const std::string& armOrRmTexPath,     // <-- packed AO/Roughness/Metallic or packed Roughness/Metallic
			const std::string& aoTexPath,
			const std::string& heightTexPath,
			const std::string& emissiveTexPath,
			const std::string& opacityTexPath);


		PBRMaterial(std::vector<Texture> _textures);

		MaterialType getTypeID() const override
		{
			return MaterialType::PBR;
		}
	};
}