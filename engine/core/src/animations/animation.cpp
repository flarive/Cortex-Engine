#include "../../include/animations/animation.h"

engine::Animation::Animation(const std::string& animationName, std::shared_ptr<Model> model, float speedFactor)
	: m_name(animationName), m_model(model), m_speedFactor(speedFactor)
{
}