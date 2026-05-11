#include "../../include/animations/animation.h"

#include "../../include/managers/log_manager.h"

engine::Animation::Animation(const std::string& animationName, std::shared_ptr<Model> model, float speedFactor)
	: m_name(animationName), m_model(model), m_speedFactor(speedFactor)
{
	logger.trace("Animation base constructor called");
}

engine::Animation::~Animation()
{
	logger.trace("Animation base destructor called");
}