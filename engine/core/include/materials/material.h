#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include <vector>
#include <string>
#include "../textures/texture.h"
#include "../shader.h"

namespace engine
{
    enum class MaterialType { undefined = 0, blinnphong = 1, PBR = 2 };

    enum class CombinedTexture { NoCombination = 0, RM = 1, ARM = 2 };
    
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
        Material(MaterialType type, const Color& ambientColor);
        Material(MaterialType type, const Color& ambientColor, const Color& diffuseColor, const Color& specularColor = Color(1.0f), float shininess = 1.0f);
        Material(MaterialType type, const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& normalTexPath, const std::string& metallicTexPath, const std::string& roughnessTexPath, const std::string& aoTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath, float shininess = 1.0f);

        Material(MaterialType type, CombinedTexture mode, const Color& ambientColor, const std::string& diffuseTexPath, const std::string& specularTexPath, const std::string& normalTexPath, const std::string& rmOrArmTexPath, const std::string& aoTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath, float shininess);

        virtual ~Material() = default;

        virtual MaterialType getTypeID() const
        {
            return MaterialType::undefined;
        }


        // parallax mapping
        void setName(const std::string& name) { m_name = name; }
        const std::string& getName() { return m_name; }

        void loadTextures();
        void loadTexturesAsync(bool flipY = false, std::function<void(bool)> texturesLoadedCallback = nullptr);

        bool bind(Shader& shader, int baseUnit = MATERIAL_BASE_UNIT) const;
        bool bind2(engine::Shader& shader) const;
        void unbind(int baseUnit = MATERIAL_BASE_UNIT) const;


        bool hasTextureMap()
        {
            if (getTypeID() == MaterialType::PBR)
            {
                return hasDiffuseMap() || hasNormalMap() || hasMetallicMap() || hasRoughnessMap() || hasAoMap() || hasArmMap() || hasRmMap() || hasHeightMap() || hasEmissiveMap() || hasOpacityMap();
            }
            else
            {
                return hasDiffuseMap() || hasSpecularMap() || hasHeightMap() || hasEmissiveMap() || hasOpacityMap();
            }
        }

        bool hasDiffuseMap() const { return !std::empty(m_diffuseTexPath); }
        bool hasSpecularMap() const { return !std::empty(m_specularTexPath); }
        bool hasNormalMap() const { return !std::empty(m_normalTexPath); }
        bool hasMetallicMap() const { return !std::empty(m_metallicTexPath); }
        bool hasRoughnessMap() const { return !std::empty(m_roughnessTexPath); }
        bool hasAoMap() const { return !std::empty(m_aoTexPath); }
        bool hasHeightMap() const { return !std::empty(m_heightTexPath); }
        bool hasEmissiveMap() const { return !std::empty(m_emissiveTexPath); }
        bool hasOpacityMap() const { return !std::empty(m_opacityTexPath); }

        bool hasArmMap() const { return !std::empty(m_armTexPath); }
        bool hasRmMap() const { return !std::empty(m_rmTexPath); }
        
        bool isCubeMap() const { return m_cubemapTextures.size() > 0; }

        bool areAllTexturesLoaded() const { return !hasTextures() || (hasTextures() && m_allTexturesLoaded); }
        void setAllTexturesLoaded(bool state) { m_allTexturesLoaded = state; }

		bool hasTextures() const { return !textures.empty(); }

        // 6 textures for the cubemap
        std::vector<std::string> m_cubemapTextures{};


        // BlinnPhong
        const engine::Color& getAmbientColor() const { return m_ambientColor; }
        const engine::Color& getDiffuseColor() const { return m_diffuseColor; }
        const engine::Color& getSpecularColor() const { return m_specularColor; }

        // PBR
        const engine::Color& getBaseColorFactor() const { return m_baseColorFactor; }
        



        const std::string& getDiffuseTexPath() const { return m_diffuseTexPath; }
        const std::string& getSpecularTexPath() const { return m_specularTexPath; }
        const std::string& getNormalTexPath() const { return m_normalTexPath; }
        const std::string& getMetallicTexPath() const { return m_metallicTexPath; }
        const std::string& getRoughnessTexPath() const { return m_roughnessTexPath; }
        const std::string& getAoTexPath() const { return m_aoTexPath; }
        const std::string& getHeightTexPath() const { return m_heightTexPath; }
        const std::string& getEmissiveTexPath() const { return m_emissiveTexPath; }
        const std::string& getOpacityTexPath() const { return m_opacityTexPath; }

        // combined textures (PBR only)
        const std::string& getArmTexPath() const { return m_armTexPath; }
        const std::string& getRmTexPath() const { return m_rmTexPath; }

        

        const int getTextureHeightUnit() const;
        
        
        float& getHeightIntensity() { return m_heightIntensity; }
        void setHeightIntensity(float height) { m_heightIntensity = height; }
        
        const float getShininessIntensity() const { return m_shininess; }
        const float getAmbientIntensity() const { return m_ambientIntensity; }
        const float getEmissiveIntensity() const { return m_emissiveIntensity; }
        
        float& getOpacityIntensity() { return m_opacityIntensity; }
        void setOpacityIntensity(float opacity) { m_opacityIntensity = opacity; }

        bool isAlphaCutOffEnabled() { return m_alphaCutoffEnabled; }
        void setAlphaCutOffEnabled(bool enabled) { m_alphaCutoffEnabled = enabled; }
        
        bool isTransparent() const;
        bool isAlphaCutout() const;


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


        unsigned int getDiffuseMapId() { return diffuseMapId; }
        unsigned int getSpecularMapId() { return specularMapId; }
        unsigned int getNormalMapId() { return normalMapId; }
        unsigned int getMetallicMapId() { return metallicMapId; }
        unsigned int getRoughnessMapId() { return roughnessMapId; }
        unsigned int getAOMapId() { return aoMapId; }
        unsigned int getHeightMapId() { return heightMapId; }
        unsigned int getEmissiveMapId() { return emissiveMapId; }
        unsigned int getOpacityMapId() { return opacityMapId; }
        
        unsigned int getArmMapId() { return armMapId; } // packed AO/Roughness/Metallic
        unsigned int getRmMapId() { return rmMapId; } // packed Roughness/Metallic

        

        void setIOR(float ior) { m_IOR = ior; }
        float& getIOR() { return m_IOR; }

    protected:
        std::string m_name{};
        
        Color m_ambientColor{ Color(0.1f) }; // blinnphong
        Color m_diffuseColor{ Color(1.0f) }; // blinnphong
        Color m_specularColor{ Color(0.0f) }; // blinnphong

        Color m_baseColorFactor{ Color(1.0f) }; // pbr

        std::string m_diffuseTexPath{};
        std::string m_specularTexPath{}; // blinnphong
        std::string m_normalTexPath{};
        
        std::string m_aoTexPath{}; // pbr
        std::string m_roughnessTexPath{}; // pbr
        std::string m_metallicTexPath{}; // pbr

        std::string m_rmTexPath{}; // pbr roughness and metallic textures combined
        std::string m_armTexPath{}; // pbr ambient occlusion, roughness and metallic textures combined
        
        std::string m_heightTexPath{}; // displacement
        std::string m_emissiveTexPath{};
        std::string m_opacityTexPath{}; // alpha texture


        // intensities
        float m_heightIntensity{};
        float m_normalIntensity{ 1.0f };
        float m_ambientIntensity{ 1.0f };
        float m_emissiveIntensity{ 1.0f };
        float m_parallaxIntensity{ 0.01f };
        float m_opacityIntensity{ 1.0f }; // fully opaque

        bool m_alphaCutoffEnabled{ false };

        bool m_useParallaxMapping{ false };

        
        bool m_allTexturesLoaded{};

        // BlinnPhong only
        float m_shininess{};

        // PBR only
        // Air          1.00
        // Water        1.33
        // Skin         1.40
        // Plastic      1.45 - 1.55
        // Glass        1.50 - 1.60
        // Quartz       1.54
        // Diamond      2.42
        float m_IOR{ 1.5f }; // For non-transparent dielectric materials (wood, plastic, concrete, leather, painted metal, skin, rubber, etc.)

    private:
        unsigned int diffuseMapId{};
        unsigned int specularMapId{};
        unsigned int normalMapId{};
        unsigned int metallicMapId{};
        unsigned int roughnessMapId{};
        unsigned int aoMapId{};
        unsigned int heightMapId{};
        unsigned int emissiveMapId{};
        unsigned int opacityMapId{};

        unsigned int armMapId{}; // packed AO/Roughness/Metallic
        unsigned int rmMapId{}; // packed Roughness/Metallic
    };
}