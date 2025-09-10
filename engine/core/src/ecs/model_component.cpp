#include "../../include/ecs/model_component.h"

#include "../../include/model.h"

engine::ModelComponent::ModelComponent(std::shared_ptr<Model> model)
    : m_model(model)
{
}


void engine::ModelComponent::init()
{

}

void engine::ModelComponent::update()
{

}

void engine::ModelComponent::draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform)
{
    m_model->draw(shader, transform);
}
