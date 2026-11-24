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
        

        // Materials begin here
        static inline constexpr int MATERIAL_BASE_UNIT = 12;


        
        Material() = default;
        Material(const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath);
        Material(CombinedTexture mode, const std::string& diffuseTexPath, const std::string& normalTexPath, const std::string& heightTexPath, const std::string& emissiveTexPath, const std::string& opacityTexPath);

        virtual ~Material() = default;

        virtual MaterialType getTypeID() const
        {
            return MaterialType::undefined;
        }


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
        

		bool hasTextures() const { return !m_textures.empty(); }


        


        
        



        const std::string& getDiffuseTexPath() const { return m_diffuseTexPath; }
        const std::string& getNormalTexPath() const { return m_normalTexPath; }
        const std::string& getHeightTexPath() const { return m_heightTexPath; }
        const std::string& getEmissiveTexPath() const { return m_emissiveTexPath; }
        const std::string& getOpacityTexPath() const { return m_opacityTexPath; }


        
        // not really used for the moment
        float& getHeightIntensity() { return m_heightIntensity; }
        void setHeightIntensity(float height) { m_heightIntensity = height; }
        
        
        
        
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


        const std::shared_ptr<Texture> getDiffuseTexture() const;
        const std::shared_ptr<Texture> getNormalTexture() const;
        const std::shared_ptr<Texture> getHeightTexture() const;
        const std::shared_ptr<Texture> getEmissiveTexture() const;
        const std::shared_ptr<Texture> getOpacityTexture() const;
        
        
        
        
        
        const std::vector<std::shared_ptr<Texture>>& getTextures() const { return m_textures; }
        void addTexture(std::shared_ptr<Texture> texture);




    protected:
        std::string m_name{};
        
        std::vector<std::shared_ptr<Texture>> m_textures{}; // Store all material textures

        
        // BlinnPhong and PBR shared texture path
        std::string m_diffuseTexPath{};
        std::string m_normalTexPath{};
        std::string m_heightTexPath{}; // displacement
        std::string m_emissiveTexPath{};
        std::string m_opacityTexPath{}; // alpha texture
        

        // BlinnPhong and PBR shared intensities
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


        private:
            // 6 textures for the cubemap
            std::vector<std::string> m_cubemapTextures{};


            virtual bool checkAllTexturesLoaded() const = 0;

            inline std::string_view getUniformSamplerName(TextureSlot type) const;
    };
}