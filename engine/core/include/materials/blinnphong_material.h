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

		bool hasSpecularMap() const { return !std::empty(m_specularTexPath); }


		const std::string& getSpecularTexPath() const { return m_specularTexPath; }

		

		
		float& getSpecularIntensity() { return m_specularIntensity; }
		void setSpecularIntensity(float intensity) { m_specularIntensity = intensity; }

		float& getShininessIntensity() { return m_shininess; }
		void setShininessIntensity(float intensity) { m_shininess = intensity; }

		bool hasTextureMap() override;


		GLuint getSpecularMapId() { return m_specularMapId; }


		void loadTextures() override;
		void loadTexturesAsync(bool flipY, std::function<void(bool)> texturesLoadedCallback) override;


	private:
		std::string m_specularTexPath{};

		float m_specularIntensity{ 1.0f };
		float m_shininess{};


		GLuint m_specularMapId{};
	};
}