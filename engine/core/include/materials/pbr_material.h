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

		PBRMaterial(const Color& baseColorFactor, const std::string& diffuseTexPath, const std::string& normalTexPath = ""
			, const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = ""
			, const std::string& heightTexPath = "", const std::string& emissiveTexPath = "", const std::string& opacityTexPath = "");

		PBRMaterial(CombinedTexture combinedTextureType, const Color& baseColorFactor,
			const std::string& diffuseTexPath,
			const std::string& normalTexPath,
			const std::string& armOrRmTexPath,     // <-- packed AO/Roughness/Metallic or packed Roughness/Metallic
			const std::string& aoTexPath = "",
			const std::string& heightTexPath = "",
			const std::string& emissiveTexPath = "",
			const std::string& opacityTexPath = "");


		//PBRMaterial(std::vector<Texture> _textures);

		MaterialType getTypeID() const override
		{
			return MaterialType::PBR;
		}

		bool hasTextureMap() override;

		// PBR
		const engine::Color& getBaseColorFactor() const { return m_baseColorFactor; }

		bool hasMetallicMap() const { return !std::empty(m_metallicTexPath); }
		bool hasRoughnessMap() const { return !std::empty(m_roughnessTexPath); }
		bool hasAoMap() const { return !std::empty(m_aoTexPath); }


		bool hasArmMap() const { return !std::empty(m_armTexPath); }
		bool hasRmMap() const { return !std::empty(m_rmTexPath); }


		const std::string& getMetallicTexPath() const { return m_metallicTexPath; }
		const std::string& getRoughnessTexPath() const { return m_roughnessTexPath; }
		const std::string& getAoTexPath() const { return m_aoTexPath; }

		// combined textures (PBR only)
		const std::string& getArmTexPath() const { return m_armTexPath; }
		const std::string& getRmTexPath() const { return m_rmTexPath; }




		float& getRougnessIntensity() { return m_roughnessIntensity; }
		void setRougnessIntensity(float intensity) { m_roughnessIntensity = intensity; }

		float& getMetallicIntensity() { return m_metallicIntensity; }
		void setMetallicIntensity(float intensity) { m_metallicIntensity = intensity; }

		float& getAmbientOcclusionIntensity() { return m_ambientOcclusionIntensity; }
		void setAmbientOcclusionIntensity(float intensity) { m_ambientOcclusionIntensity = intensity; }


		GLuint getMetallicMapId() { return m_metallicMapId; }
		GLuint getRoughnessMapId() { return m_roughnessMapId; }
		GLuint getAOMapId() { return m_aoMapId; }

		GLuint getArmMapId() { return m_armMapId; } // packed AO/Roughness/Metallic
		GLuint getRmMapId() { return m_rmMapId; } // packed Roughness/Metallic


		void setIOR(float ior) { m_IOR = ior; }
		float& getIOR() { return m_IOR; }

		void setTransmission(float transmission) { m_transmission = transmission; }
		float& getTransmission() { return m_transmission; }
		

		void setThickness(float thickness) { m_thickness = thickness; }
		float& getThickness() { return m_thickness; }

		void setAttenuationColor(const Color& attenuationColor) { m_attenuationColor = attenuationColor; }
		Color& getAttenuationColor() { return m_attenuationColor; }

		void setAttenuationDistance(float attenuationDistance) { m_attenuationDistance = attenuationDistance; }
		float& getAttenuationDistance() { return m_attenuationDistance; }


		void loadTextures() override;
		void loadTexturesAsync(bool flipY, std::function<void(bool)> texturesLoadedCallback) override;


	private:
		Color m_baseColorFactor{ Color(1.0f) };

		std::string m_aoTexPath{};
		std::string m_roughnessTexPath{};
		std::string m_metallicTexPath{};

		std::string m_rmTexPath{}; // pbr roughness and metallic textures combined
		std::string m_armTexPath{}; // pbr ambient occlusion, roughness and metallic textures combined


		float m_roughnessIntensity{ 1.0f };
		float m_metallicIntensity{ 1.0f };
		float m_ambientOcclusionIntensity{ 1.0f };




		// PBR only
		// Air          1.00
		// Water        1.33
		// Skin         1.40
		// Plastic      1.45 - 1.55
		// Glass        1.50 - 1.60
		// Quartz       1.54
		// Diamond      2.42
		float m_IOR{ 1.5f }; // For non-transparent dielectric materials (wood, plastic, concrete, leather, painted metal, skin, rubber, etc.)
		//float m_transmission{ 0.0f }; // 0 = opaque, 1 = fully transmissive
		float m_thickness{ 0.0f }; // object thickness in meters
		Color m_attenuationColor{};
		float m_attenuationDistance{ 0.0f };





		GLuint m_metallicMapId{};
		GLuint m_roughnessMapId{};
		GLuint m_aoMapId{};

		GLuint m_armMapId{}; // packed AO/Roughness/Metallic
		GLuint m_rmMapId{}; // packed Roughness/Metallic
	};
}