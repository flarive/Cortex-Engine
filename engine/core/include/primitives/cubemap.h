#pragma once

#include "../misc/noncopyable.h"
#include "../shader.h"

#include <vector>

namespace engine
{
    class Cubemap : private NonCopyable
    {
    private:
        unsigned int envCubemap;



    public:
        Cubemap() = default;
        ~Cubemap() = default;

        void setup(const std::vector<std::string>& faces);

        // draws the model, and thus all its meshes
        void draw(const glm::mat4& projection, const glm::mat4& view);

        // optional: de-allocate all resources once they've outlived their purpose
        void clean();
    };
}
