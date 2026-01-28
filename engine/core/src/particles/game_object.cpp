#include "../../include/particles/game_object.h"

engine::GameObject::GameObject() 
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), m_spritePath(), IsSolid(false), Destroyed(false)
{
}

engine::GameObject::GameObject(const glm::vec2& pos, const glm::vec2& size, const std::string& spritePath, const glm::vec3& color, const glm::vec2& velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), m_spritePath(spritePath), IsSolid(false), Destroyed(false)
{
	m_textureId = Texture::loadGLTextureFromFile(spritePath.c_str(), "", true, true, true);
}

void engine::GameObject::Draw(SpriteRenderer &renderer)
{
    renderer.DrawSprite(this->m_textureId, this->Position, this->Size, this->Rotation, this->Color);
}