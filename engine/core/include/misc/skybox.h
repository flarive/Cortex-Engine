#pragma once

#include "../misc/noncopyable.h"
#include "../shaders/shader.h"

#include <vector>

namespace engine
{
    class Skybox final : private NonCopyable
    {
    private:
        unsigned int m_cubemapTexture{};
        unsigned int m_skyboxVAO{}, m_skyboxVBO{};
        std::unique_ptr<ShaderBase> m_skyboxShader{};

		bool m_isSetup{};

    public:
        Skybox() = default;
        ~Skybox() = default;

        void setup(const std::vector<std::string>& faces);

        // draws the model, and thus all its meshes
        void draw(const glm::mat4& projection, const glm::mat4& view);

        // optional: de-allocate all resources once they've outlived their purpose
        void clean();
    };
}
