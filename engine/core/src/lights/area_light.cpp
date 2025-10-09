#include "../../include/lights/area_light.h"

#include "../../include/singleton.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles

#include <format>


engine::AreaLight::AreaLight(unsigned int index) : Light(glm::vec3(), index)
{
}

engine::AreaLight::AreaLight(glm::vec3 _position, unsigned int index) : Light(_position, index)
{
    setup();
}

void engine::AreaLight::setup()
{
    // load light cube debug shader
    m_lightDebugShader.init("light_cube", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");


    auto matDebugLight = std::make_shared<engine::Material>(engine::Color(1.0f, 1.0f, 1.0f, 0.2f));
    m_debug_cylinder.setup(matDebugLight);
}

void engine::AreaLight::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix)
{
    std::string base = std::format("areaLights[{}]", m_index);

    // directional light
    shader.setBool(std::format("{}.use", base), true);

    shader.setVec3(std::format("{}.position", base), position);

    shader.setVec3(std::format("{}.ambient", base), ambient);
    shader.setVec3(std::format("{}.diffuse", base), diffuse * intensity);
    shader.setVec3(std::format("{}.specular", base), specular);

    shader.setVec3(std::format("{}.direction", base), calculateLightDirection(position, target));


    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    if (sceneSettings.drawLightsVisualHelpers)
    {
        glm::vec3 direction = glm::normalize(target - position);
        glm::vec3 defaultAxis = glm::vec3(0.0f, 1.0f, 0.0f); // cylinder points up

        // Compute quaternion rotation between default axis and desired direction
        glm::quat rotationQuat = glm::rotation(defaultAxis, direction);

        // Convert to rotation matrix
        glm::mat4 rotationMatrix = glm::toMat4(rotationQuat);

        // Compose final model matrix
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model *= rotationMatrix;
        model = glm::scale(model, glm::vec3(0.05f, glm::length(target - position), 0.05f)); // scale lengthwise toward target

        // Pass model matrix to shader
        m_lightDebugShader.use();
        m_lightDebugShader.setMat4("projection", projection);
        m_lightDebugShader.setMat4("view", view);
        m_lightDebugShader.setVec4("customColor", m_debug_cylinder.getMaterial()->getAmbientColor());

        // You can pass glm::vec3(0) for rotation since model is already transformed
        auto localTransform = Transform(position, glm::vec3(0.0f), glm::vec3(1.0f));
        m_debug_cylinder.draw(m_lightDebugShader, model, localTransform);
    }
}

void engine::AreaLight::clean()
{
}