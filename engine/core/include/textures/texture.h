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
        TextureFlag_GammaCorrect = 1 << 3
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


    /// <summary>
    /// PNG/JPG → small file on disk
    /// BC7/BC5/BC4 → small texture in VRAM
    /// </summary>
    class Texture final : private NonCopyableButMovable
    {
    public:
        unsigned int id{};
        std::string type{};
        std::string path{};
        Color color{};
        int thumbnailLevel{};   // chosen mipmap level for thumbnails
        

		Texture() = default;
        Texture(unsigned int id, const std::string& type, const std::string& path);
		~Texture() = default;

        // override a little NonCopyableButMovable
        Texture(Texture&&) = default;
        
		// tomake it really movable, we need to implement the move assignment operator
        //Texture& operator=(Texture&& other) noexcept
        //{
        //    if (this != &other)
        //    {
        //        id = other.id;
        //        type = std::move(other.type);
        //        path = std::move(other.path);
        //        color = other.color;
        //        thumbnailLevel = other.thumbnailLevel;

        //        other.id = 0;
        //    }
        //    return *this;
        //}
    };
}
