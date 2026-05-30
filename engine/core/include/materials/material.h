#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include <vector>
#include <string>
#include "../texture.h"
#include "../shader.h"

namespace engine
{
    enum class MaterialType { undefined = 0, blinnphong = 1, PBR = 2 };
    
    /// <summary>
    /// Abstract class for materials
    /// </summary>
    class Material : public NonCopyable
    {
    public:
        std::vector<Texture> textures{}; // Store all material textures

        // Materials begin here
        static inline constexpr int MATERIAL_BASE_UNIT = 12;


        Material(std::vector<Texture> _textures, float _shininess = 1.0f);
        Material(const Color& ambientColor);
        Material(const Color& ambientColor, const Color& diffuseColor, const Color& specularColor = Color(1.0f), float shininess = 1.0f);
        Material(const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath = "", const std::string& normalTexPath = "", const std::string& metallicTexPath = "", const std::string& roughnessTexPath = "", const std::string& aoTexPath = "", const std::string& heightTexPath = "", float shininess = 1.0f);

        virtual ~Material() = default;

        virtual MaterialType getTypeID() const
        {
            return MaterialType::undefined;
        }

        void loadTextures();
        void loadTexturesAsync(std::function<void(bool)> texturesLoaded = nullptr);

        bool bind(Shader& shader, int baseUnit = MATERIAL_BASE_UNIT) const;
        bool bind2(engine::Shader& shader) const;
        void unbind(int baseUnit = MATERIAL_BASE_UNIT) const;


        bool hasDiffuseMap() const { return !std::empty(m_diffuseTexPath); }
        bool hasSpecularMap() const { return !std::empty(m_specularTexPath); }
        bool hasNormalMap() const { return !std::empty(m_normalTexPath); }
        bool hasMetallicMap() const { return !std::empty(m_metallicTexPath); }
        bool hasRoughnessMap() const { return !std::empty(m_roughnessTexPath); }
        bool hasAoMap() const { return !std::empty(m_aoTexPath); }
        bool hasHeightMap() const { return !std::empty(m_heightTexPath); }
        bool hasEmissiveMap() const { return !std::empty(m_emissiveTexPath); }

        bool isCubeMap() const { return m_cubemapTextures.size() > 0; }

        bool areAllTexturesLoaded() const { return !hasTextures() || (hasTextures() && m_allTexturesLoaded); }
        void setAllTexturesLoaded(bool state) { m_allTexturesLoaded = state; }

		bool hasTextures() const { return !textures.empty(); }

        // 6 textures for the cubemap
        std::vector<std::string> m_cubemapTextures{};



        const engine::Color& getAmbientColor() const { return m_ambientColor; }
        const engine::Color& getDiffuseColor() const { return m_diffuseColor; }
        const engine::Color& getSpecularColor() const { return m_specularColor; }

        const std::string& getDiffuseTexPath() const { return m_diffuseTexPath; }
        const std::string& getSpecularTexPath() const { return m_specularTexPath; }
        const std::string& getNormalTexPath() const { return m_normalTexPath; }
        const std::string& getMetallicTexPath() const { return m_metallicTexPath; }
        const std::string& getRoughnessTexPath() const { return m_roughnessTexPath; }
        const std::string& getAoTexPath() const { return m_aoTexPath; }
        const std::string& getHeightTexPath() const { return m_heightTexPath; }
        const std::string& getEmissiveTexPath() const { return m_emissiveTexPath; }

        const int getTextureHeightUnit() const;
        
        
        float& getHeightIntensity() { return m_heightIntensity; }
        void setHeightIntensity(float height) { m_heightIntensity = height; }
        
        const float getShininessIntensity() const { return m_shininess; }
        const float getAmbientIntensity() const { return m_ambientIntensity; }
        const float getEmissiveIntensity() const { return m_emissiveIntensity; }


        void setShininessIntensity(float intensity) { m_shininess = intensity; }
        void setAmbientIntensity(float intensity) { m_ambientIntensity = intensity; }


        // normal mapping
        float& getNormalIntensity() { return m_normalIntensity; }
        void setNormalIntensity(float intensity) { m_normalIntensity = intensity; }
        
        
        
        void setEmissiveIntensity(float intensity) { m_emissiveIntensity = intensity; }


        // parallax mapping
        void useParallaxMapping(bool use) { m_useParallaxMapping = use; }
        bool& useParallaxMapping() { return m_useParallaxMapping; }

        void setParallaxIntensity(float intensity) { m_parallaxIntensity = intensity; }
        float& getParallaxIntensity() { return m_parallaxIntensity; }

        const std::vector<std::string>& getCubeMapTexs() const { return m_cubemapTextures; }

        void setCubeMapTexs(const std::vector<std::string>& faces);



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


        // intensities
        float m_heightIntensity{};
        float m_normalIntensity{ 1.0f };
        float m_ambientIntensity{ 1.0f };
        float m_emissiveIntensity{ 1.0f };
        float m_parallaxIntensity{ 0.01f };


        bool m_useParallaxMapping{ false };

        float m_shininess{};
        bool m_allTexturesLoaded{};

    private:
        unsigned int heightMapId{};
    };
}