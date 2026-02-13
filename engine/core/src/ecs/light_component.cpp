#include "../../include/ecs/light_component.h"



engine::LightComponent::LightComponent(std::shared_ptr<Light> light)
    : m_light(light)
{
    m_boundingVolume = std::make_unique<AABB>(generateBoundingVolume(light));

    // Initialize property setters based on primitive type
    m_propertySetters = m_light->getPropertySetters();
}

void engine::LightComponent::init(Transform& transform)
{
    m_light->position = transform.getLocalPosition();
}

void engine::LightComponent::update(float deltaTime, Transform& transform)
{

}

void engine::LightComponent::draw(const glm::mat4& projection, const glm::mat4& view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume)
{
    m_light->draw(shader,
        projection,
        view,
        m_light->ambientColor,
        m_light->diffuseColor,
        m_light->specularColor,
        m_light->intensity,
        m_light->target,
        worldTransformMatrix,
        localTransform);
}

engine::AABB engine::LightComponent::generateBoundingVolume(const std::shared_ptr<Light> light)
{
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

    return engine::AABB(minAABB, maxAABB);
}

engine::AABB* engine::LightComponent::getBoundingVolume()
{
    return m_boundingVolume.get();
}

engine::ordered_map<std::string, engine::EditorProperty> engine::LightComponent::getPublicProperties()
{
    return m_light->getPublicProperties();
}

std::unordered_map<std::string, std::function<void(engine::EditorPropertyValue)>> engine::LightComponent::getPropertySetters()
{
    return m_light->getPropertySetters();
}

void engine::LightComponent::setProperty(const std::string& key, engine::EditorPropertyValue value)
{
    auto it = m_propertySetters.find(key);
    if (it != m_propertySetters.end())
    {
        it->second(value);
        //m_light->reSetup(); // Assuming all primitives have a reSetup() method
    }
}

void engine::LightComponent::setEnabled(bool enabled)
{
    ComponentBase<LightComponent>::setEnabled(enabled);

	m_light->setEnabled(enabled);
}