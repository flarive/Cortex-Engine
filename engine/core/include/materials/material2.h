#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"

#include <vector>
#include <string>
#include "..\shader.h"
#include "..\texture.h"

namespace engine
{
    class Material2 {
    public:
        std::vector<Texture> textures; // Store textures

        Material2(const std::vector<Texture>& textures) : textures(textures) {}

        void bind(Shader& shader) const;
        void unbind() const;
    };
}

