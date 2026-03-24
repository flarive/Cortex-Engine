#include "../../include/animations/animation.h"

engine::Animation::Animation(const std::string& animationName, const std::string& animationPath, std::shared_ptr<Model> model, float speedFactor)
	: m_name(animationName), m_model(model), m_speedFactor(speedFactor)
{
}

engine::Animation::Animation(const std::string& animationName, const glm::vec3& rotation, std::shared_ptr<Model> model, float speedFactor)
	: m_name(animationName), m_model(model), m_speedFactor(speedFactor)
{
}