#pragma once

#include <string>
#include <vector>

#include "../misc/noncopyable.h"
#include "../common_defines.h"

namespace engine
{
    /// <summary>
    /// Abstract class for materials
    /// </summary>
    class Material : private NonCopyable
    {
    private:
        engine::Color m_ambientColor{ engine::Color(0.0f) };
        engine::Color m_diffuseColor{ engine::Color(0.5f) };

        std::string m_diffuseTexPath{};
        std::string m_specularTexPath{};
        std::string m_normalTexPath{};
        std::string m_metallicTexPath{};
        std::string m_roughnessTexPath{};
        std::string m_aoTexPath{};
        std::string m_heightTexPath{};

        // 6 textures for the cubemap
        std::vector<std::string> m_cubemapTextures{};

        float m_shininess{};

    public:
        Material() = default;
        Material(const engine::Color& ambientColor, const engine::Color& diffuseColor, float shininess = 32.0f);

        Material(const engine::Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = "", const std::string& heightTexPath = "", float shininess = 32.0f);

        ~Material() = default;

		const engine::Color& getAmbientColor() const { return m_ambientColor; }
        const engine::Color& getDiffuseColor() const { return m_diffuseColor; }

		const std::string& getDiffuseTexPath() const { return m_diffuseTexPath; }
        const std::string& getSpecularTexPath() const { return m_specularTexPath; }
        const std::string& getNormalTexPath() const { return m_normalTexPath; }
        const std::string& getMetallicTexPath() const { return m_metallicTexPath; }
        const std::string& getRoughnessTexPath() const { return m_roughnessTexPath; }
        const std::string& getAoTexPath() const { return m_aoTexPath; }
        const std::string& getHeightTexPath() const { return m_heightTexPath; }


		const std::vector<std::string>& getCubeMapTexs() const { return m_cubemapTextures; }

        void setCubeMapTexs(const std::vector<std::string>& faces);

		float getShininess() const { return m_shininess; }



		bool hasDiffuseMap() const { return !std::empty(m_diffuseTexPath); }
		bool hasSpecularMap() const { return !std::empty(m_specularTexPath); }
		bool hasNormalMap() const { return !std::empty(m_normalTexPath); }
        bool hasMetallicMap() const { return !std::empty(m_metallicTexPath); }
        bool hasRoughnessMap() const { return !std::empty(m_roughnessTexPath); }
        bool hasAoMap() const { return !std::empty(m_aoTexPath); }
        bool hasHeightMap() const { return !std::empty(m_heightTexPath); }

		bool isCubeMap() const { return m_cubemapTextures.size() > 0; }
    };
}