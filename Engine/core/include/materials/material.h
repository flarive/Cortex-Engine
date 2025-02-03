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

        Material(const engine::color& ambientColor, const engine::color& diffuseColor, float shininess = 32.0f);

        Material(const engine::color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", float shininess = 32.0f);


        virtual ~Material() = default;

		engine::color getAmbientColor() const { return m_ambientColor; }
		engine::color getDiffuseColor() const { return m_diffuseColor; }

		std::string getDiffuseTexPath() const { return m_diffuseTexPath; }
		std::string getSpecularTexPath() const { return m_specularTexPath; }
		std::string getNormalTexPath() const { return m_normalTexPath; }
		std::vector<std::string> getCubeMapTexs() const { return m_cubemapTextures; }

		float getShininess() const { return m_shininess; }



		bool hasDiffuseMap() const { return !std::empty(m_diffuseTexPath); }
		bool hasSpecularMap() const { return !std::empty(m_specularTexPath); }
		bool hasNormalMap() const { return !std::empty(m_normalTexPath); }
		bool hasCubeMap() const { return m_cubemapTextures.size() > 0; }

        

    private:
		engine::color m_ambientColor = engine::color(0.0f);
        engine::color m_diffuseColor = engine::color(0.5f);
		
        std::string m_diffuseTexPath;
        std::string m_specularTexPath;
        std::string m_normalTexPath;

		// 6 textures for the cubemap
        std::vector<std::string> m_cubemapTextures;

        float m_shininess = 0.0f;
    };
}