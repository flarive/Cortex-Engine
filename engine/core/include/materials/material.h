#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include <vector>
#include <string>
#include "../texture.h"
#include "../shader.h"

namespace engine
{
    /// <summary>
    /// Abstract class for materials
    /// </summary>
    class Material : public NonCopyable
    {
    public:
        std::vector<Texture> textures{}; // Store textures

        Material(std::vector<Texture> _textures, float _shininess = 1.0f);
        Material(const Color& ambientColor);
        Material(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor = Color(1.0f), float shininess = 1.0f);
        Material(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = "", const std::string& heightTexPath = "", float shininess = 1.0f);

        virtual ~Material() = default;

        virtual void loadTextures();
        virtual void loadTexturesAsync();


        virtual bool bind(Shader& shader, unsigned int baseUnit = 12) const; // TODO !!!! use MATERIAL_BASE_UNIT
        virtual bool bind2(engine::Shader& shader) const;
        virtual void unbind(int baseUnit = 12) const; // TODO !!!! use MATERIAL_BASE_UNIT


        virtual bool hasDiffuseMap() const { return !std::empty(m_diffuseTexPath); }
        virtual bool hasSpecularMap() const { return !std::empty(m_specularTexPath); }
        virtual bool hasNormalMap() const { return !std::empty(m_normalTexPath); }
        virtual bool hasMetallicMap() const { return !std::empty(m_metallicTexPath); }
        virtual bool hasRoughnessMap() const { return !std::empty(m_roughnessTexPath); }
        virtual bool hasAoMap() const { return !std::empty(m_aoTexPath); }
        virtual bool hasHeightMap() const { return !std::empty(m_heightTexPath); }
        virtual bool hasEmissiveMap() const { return !std::empty(m_emissiveTexPath); }

        virtual bool isCubeMap() const { return m_cubemapTextures.size() > 0; }

        bool areAllTexturesLoaded() const { return !hasTextures() || (hasTextures() && m_allTexturesLoaded); }
        void setAllTexturesLoaded(bool state) { m_allTexturesLoaded = state; }

		bool hasTextures() const { return !textures.empty(); }

        // 6 textures for the cubemap
        std::vector<std::string> m_cubemapTextures{};



        const virtual engine::Color& getAmbientColor() const { return m_ambientColor; }
        const virtual engine::Color& getDiffuseColor() const { return m_diffuseColor; }
        const virtual engine::Color& getSpecularColor() const { return m_specularColor; }

        const virtual std::string& getDiffuseTexPath() const { return m_diffuseTexPath; }
        const virtual std::string& getSpecularTexPath() const { return m_specularTexPath; }
        const virtual std::string& getNormalTexPath() const { return m_normalTexPath; }
        const virtual std::string& getMetallicTexPath() const { return m_metallicTexPath; }
        const virtual std::string& getRoughnessTexPath() const { return m_roughnessTexPath; }
        const virtual std::string& getAoTexPath() const { return m_aoTexPath; }
        const virtual std::string& getHeightTexPath() const { return m_heightTexPath; }
        const virtual std::string& getEmissiveTexPath() const { return m_emissiveTexPath; }


        const virtual float getNormalIntensity() const { return m_normalIntensity; }
        const virtual float getHeightIntensity() const { return m_heightIntensity; }
        const virtual float getShininessIntensity() const { return m_shininess; }
        const virtual float getAmbientIntensity() const { return m_ambientIntensity; }
        const virtual float getEmissiveIntensity() const { return m_emissiveIntensity; }


        virtual void setNormalIntensity(float intensity) { m_normalIntensity = intensity; }
        virtual void setHeightIntensity(float height) { m_heightIntensity = height; }
        virtual void setShininessIntensity(float intensity) { m_shininess = intensity; }
        virtual void setAmbientIntensity(float intensity) { m_ambientIntensity = intensity; }
        virtual void setEmissiveIntensity(float intensity) { m_emissiveIntensity = intensity; }

        const virtual std::vector<std::string>& getCubeMapTexs() const { return m_cubemapTextures; }

        virtual void setCubeMapTexs(const std::vector<std::string>& faces);


    protected:

        Color m_ambientColor{ Color(0.1f) };
        Color m_diffuseColor{ Color(1.0f) };
        Color m_specularColor{ Color(0.0f) };

        std::string m_diffuseTexPath{};
        std::string m_specularTexPath{};
        std::string m_normalTexPath{};
        std::string m_metallicTexPath{};
        std::string m_roughnessTexPath{};
        std::string m_aoTexPath{};
        std::string m_heightTexPath{};
        std::string m_emissiveTexPath{};


        float m_heightIntensity{};
        float m_normalIntensity{ 1.0f };
        float m_ambientIntensity{ 1.0f };
        float m_emissiveIntensity{ 1.0f };

        float m_shininess{};

		//bool m_canCastShadows{ true };
  //      bool m_canReceiveShadows{ true };

        bool m_allTexturesLoaded{};

        
    };
}