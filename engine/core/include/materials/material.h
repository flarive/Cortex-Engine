#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include <vector>
#include <string>
#include "../shader.h"
#include "../texture.h"

namespace engine
{
    class Material : public NonCopyable
    {
    protected:
        
        Color m_ambientColor{ engine::Color(0.0f) };
        Color m_diffuseColor{ engine::Color(0.5f) };

        std::string m_diffuseTexPath{};
        std::string m_specularTexPath{};
        std::string m_normalTexPath{};
        std::string m_metallicTexPath{};
        std::string m_roughnessTexPath{};
        std::string m_aoTexPath{};
        std::string m_heightTexPath{};


        float m_shininess{};
    
    public:
        std::vector<Texture> textures{}; // Store textures

        Material(const std::vector<Texture>& textures);

        Material(const engine::Color& ambientColor, const engine::Color& diffuseColor, float shininess = 0.0f);

        Material(const engine::Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = "", const std::string& heightTexPath = "", float shininess = 0.0f);


        void loadTextures();

        void bind(Shader& shader) const;
        void unbind() const;

        bool hasDiffuseMap() const { return !std::empty(m_diffuseTexPath); }
        bool hasSpecularMap() const { return !std::empty(m_specularTexPath); }
        bool hasNormalMap() const { return !std::empty(m_normalTexPath); }
        bool hasMetallicMap() const { return !std::empty(m_metallicTexPath); }
        bool hasRoughnessMap() const { return !std::empty(m_roughnessTexPath); }
        bool hasAoMap() const { return !std::empty(m_aoTexPath); }
        bool hasHeightMap() const { return !std::empty(m_heightTexPath); }

        bool isCubeMap() const { return m_cubemapTextures.size() > 0; }

        // 6 textures for the cubemap
        std::vector<std::string> m_cubemapTextures{};



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
    };
}