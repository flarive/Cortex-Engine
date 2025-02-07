#pragma once

#include <string>
#include <vector>

#include "../common_defines.h"

namespace engine
{
    /// <summary>
    /// Abstract class for materials
    /// </summary>
    class Material
    {
    public:

        Material();

        Material(const engine::Color& ambientColor, const engine::Color& diffuseColor, float shininess = 32.0f);

        Material(const engine::Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", float shininess = 32.0f);


        virtual ~Material() = default;

		engine::Color getAmbientColor() const { return m_ambientColor; }
		engine::Color getDiffuseColor() const { return m_diffuseColor; }

		std::string getDiffuseTexPath() const { return m_diffuseTexPath; }
		std::string getSpecularTexPath() const { return m_specularTexPath; }
		std::string getNormalTexPath() const { return m_normalTexPath; }
		std::vector<std::string> getCubeMapTexs() const { return m_cubemapTextures; }

        void setCubeMapTexs(std::vector<std::string> faces);

		float getShininess() const { return m_shininess; }



		bool hasDiffuseMap() const { return !std::empty(m_diffuseTexPath); }
		bool hasSpecularMap() const { return !std::empty(m_specularTexPath); }
		bool hasNormalMap() const { return !std::empty(m_normalTexPath); }
		bool hasCubeMap() const { return m_cubemapTextures.size() > 0; }

        

    private:
		engine::Color m_ambientColor = engine::Color(0.0f);
        engine::Color m_diffuseColor = engine::Color(0.5f);
		
        std::string m_diffuseTexPath;
        std::string m_specularTexPath;
        std::string m_normalTexPath;

		// 6 textures for the cubemap
        std::vector<std::string> m_cubemapTextures;

        float m_shininess = 0.0f;
    };
}