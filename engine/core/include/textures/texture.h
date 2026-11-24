#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

namespace engine
{
    enum TextureFlags : uint32_t
    {
        TextureFlag_None = 0,
        TextureFlag_InvertY = 1 << 0,
        TextureFlag_GenerateMipmaps = 1 << 1,
        TextureFlag_RepeatTexture = 1 << 2,
        TextureFlag_GammaCorrect = 1 << 3,
        TextureFlag_sRGB = 1 << 4
    };

    

    inline TextureFlags operator|(TextureFlags a, TextureFlags b)
    {
        return static_cast<TextureFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline TextureFlags operator&(TextureFlags a, TextureFlags b)
    {
        return static_cast<TextureFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline TextureFlags& operator|=(TextureFlags& a, TextureFlags b)
    {
        a = a | b;
        return a;
    }

    inline bool hasFlag(TextureFlags flags, TextureFlags flag)
    {
        return (flags & flag) != 0;
    }

    enum class TextureSlot : uint8_t
    {
        Unknown,
        Diffuse,
        Normal,
        AO,
        Roughness,
        Metallic,
        Height,
        Emissive,
        Opacity,
        Specular,
        ARM,
        RM
    };

    class Texture : private NonCopyableButMovable
    {
    public:
        GLuint id{};
        TextureSlot type{};
        std::string path{};
        Color color{};
        int thumbnailLevel{};   // chosen mipmap level for thumbnails
        

		Texture() = default;
        Texture(GLuint id, const TextureSlot& type, const std::string& path);
		~Texture() = default;

        // override a little NonCopyableButMovable
        Texture(Texture&&) = default;
    };

    class VulkanTexture final : public Texture
    {
    public:
        VkImage image{};
        VkDeviceMemory memory{};
        VkImageView imageView{};
        VkSampler sampler{};
    };

    class OpenGLTexture final : public Texture
    {
    
        
    };

    //class Texture
    //{
    //public:
    //    virtual ~Texture() = default;
    //};
}
