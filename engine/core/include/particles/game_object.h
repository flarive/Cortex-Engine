#pragma once


#include "../common_defines.h"
#include "../texture.h"
#include "sprite_renderer.h"


namespace engine
{
    // Container object for holding all state relevant for a single
    // game object entity. Each object in the game likely needs the
    // minimal of state as described within GameObject.
    class GameObject
    {
    public:
        // object state
        glm::vec2   Position{}, Size{}, Velocity{};
        glm::vec3   Color{};
        float       Rotation{};
        bool        IsSolid{};
        bool        Destroyed{};
        // render state
        unsigned int m_textureId{};
		std::string m_spritePath{};
        // constructor(s)
        GameObject();
        GameObject(const glm::vec2& pos, const glm::vec2& size, const std::string& spritePath, const glm::vec3& color = glm::vec3(1.0f), const glm::vec2& velocity = glm::vec2(0.0f, 0.0f));
        // draw sprite
        virtual void Draw(SpriteRenderer& renderer);
    };
}