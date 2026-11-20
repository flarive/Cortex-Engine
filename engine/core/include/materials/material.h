#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include <vector>
#include <string>
#include "../textures/texture.h"
#include "../shaders/shader.h"

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


        Material(std::vector<Texture> _textures);
        Material(MaterialType type, const Color& ambientColor);
        Material(MaterialType type, const Color& ambientColor, const Color& diffuseColor, const Color& specularColor = Color(1.0f));
        Material(MaterialType type, const Color& ambientColor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath);
        Material(MaterialType type, CombinedTexture mode, const Color& ambientColor, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath);

        virtual ~Material() = default;

        virtual MaterialType getTypeID() const
        {
            return MaterialType::undefined;
        }


        // parallax mapping
        void setName(const std::string& name) { m_name = name; }
        const std::string& getName() { return m_name; }

        virtual void loadTextures() = 0;
        virtual void loadTexturesAsync(bool flipY = false, std::function<void(bool)> texturesLoadedCallback = nullptr) = 0;

        bool bind(std::unique_ptr<Shader>& shader, int baseUnit = MATERIAL_BASE_UNIT) const;
        bool bind2(std::unique_ptr<Shader>& shader) const;
        void unbind(int baseUnit = MATERIAL_BASE_UNIT) const;


        virtual bool hasTextureMap() = 0;


        bool hasDiffuseMap() const { return !std::empty(m_diffuseTexPath); }
        bool hasNormalMap() const { return !std::empty(m_normalTexPath); }
        bool hasHeightMap() const { return !std::empty(m_heightTexPath); }
        bool hasEmissiveMap() const { return !std::empty(m_emissiveTexPath); }
        bool hasOpacityMap() const { return !std::empty(m_opacityTexPath); }

        
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
        const std::string& getNormalTexPath() const { return m_normalTexPath; }
        const std::string& getHeightTexPath() const { return m_heightTexPath; }
        const std::string& getEmissiveTexPath() const { return m_emissiveTexPath; }
        const std::string& getOpacityTexPath() const { return m_opacityTexPath; }


        
        // not really used for the moment
        float& getHeightIntensity() { return m_heightIntensity; }
        void setHeightIntensity(float height) { m_heightIntensity = height; }
        
        
        const float getAmbientIntensity() const { return m_ambientIntensity; }
        
        float& getEmissiveStrength() { return m_emissiveStrength; }
        void setEmissiveStrength(float strength) { m_emissiveStrength = strength; }

        Color& getEmissiveColorFactor() { return m_emissiveColorFactor; }
        void setEmissiveColorFactor(const Color& factor) { m_emissiveColorFactor = factor; }
    
        float& getOpacityIntensity() { return m_opacityIntensity; }
        void setOpacityIntensity(float opacity) { m_opacityIntensity = opacity; }

        bool isAlphaCutOffEnabled() { return m_alphaCutoffEnabled; }
        void setAlphaCutOffEnabled(bool enabled) { m_alphaCutoffEnabled = enabled; }
        
        bool isTransparent() const;
        bool isAlphaCutout() const;


        
        void setAmbientIntensity(float intensity) { m_ambientIntensity = intensity; }




        // normal mapping
        float& getNormalIntensity() { return m_normalIntensity; }
        void setNormalIntensity(float intensity) { m_normalIntensity = intensity; }

        

        


        // parallax mapping
        void useParallaxMapping(bool use) { m_useParallaxMapping = use; }
        bool& useParallaxMapping() { return m_useParallaxMapping; }

        void setParallaxIntensity(float intensity) { m_parallaxIntensity = intensity; }
        float& getParallaxIntensity() { return m_parallaxIntensity; }


        bool hasTransmission() { return m_transmission > 0.0f; }


        const std::vector<std::string>& getCubeMapTexs() const { return m_cubemapTextures; }

        void setCubeMapTexs(const std::vector<std::string>& faces);


        GLuint getDiffuseMapId() { return m_diffuseMapId; }
        GLuint getNormalMapId() { return m_normalMapId; }
        GLuint getHeightMapId() { return m_heightMapId; }
        GLuint getEmissiveMapId() { return m_emissiveMapId; }
        GLuint getOpacityMapId() { return m_opacityMapId; }
        
        

        




    protected:
        std::string m_name{};
        
        // BlinnPhong and PBR shared properties
        Color m_ambientColor{ Color(0.1f) };
        Color m_diffuseColor{ Color(1.0f) };
        Color m_specularColor{ Color(0.0f) };

        
        // BlinnPhong and PBR shared texture path
        std::string m_diffuseTexPath{};
        std::string m_normalTexPath{};
        std::string m_heightTexPath{}; // displacement
        std::string m_emissiveTexPath{};
        std::string m_opacityTexPath{}; // alpha texture
        

        // BlinnPhong and PBR shared intensities
        float m_ambientIntensity{ 1.0f };
        float m_normalIntensity{ 1.0f };
        float m_heightIntensity{ 1.0f };
        Color m_emissiveColorFactor{};
        float m_emissiveStrength{ 0.0f };
        float m_opacityIntensity{ 1.0f }; // fully opaque



        // PBR only but shared to avoid casting
        float m_transmission{ 0.0f }; // 0 = opaque, 1 = fully transmissive

        bool m_alphaCutoffEnabled{ false };

        bool m_useParallaxMapping{ false };
        float m_parallaxIntensity{ 0.01f };

        
        bool m_allTexturesLoaded{};


        GLuint m_diffuseMapId{};
        GLuint m_normalMapId{};
        GLuint m_heightMapId{};
        GLuint m_emissiveMapId{};
        GLuint m_opacityMapId{};
    };
}