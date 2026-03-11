#include "../../include/lights/spot_light.h"

#include "../../include/singleton.h"

#include <format>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles

engine::SpotLight::SpotLight() : SpotLight(glm::vec3())
{
}

engine::SpotLight::SpotLight(glm::vec3 _position) : Light(_position)
{
    setup();
}

void engine::SpotLight::setup()
{
    // load light cube debug shader
    m_lightDebugShader.init("light_cube", "shaders/debug/debug_light.vert", "shaders/debug/debug_light.frag");

    auto matDebugLight = std::make_shared<engine::Material>(engine::Color(1.0f, 1.0f, 1.0f, 0.2f));
    m_debug_cone.setup(matDebugLight);
}

void engine::SpotLight::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, const Color& diffuse, const Color& specular, float intensity, const glm::vec3& target, const glm::mat4 transformMatrix, Transform& localTransform)
{
    ShaderType type = shader.getShaderType();

    if (type == ShaderType::BlinnPhong || type == ShaderType::BlinnPhongTessellation
        || type == ShaderType::PBR)
    {
        std::string base = std::format("spotLights[{}]", m_index);
        
        shader.use();
        shader.setBool(std::format("{}.use", base), getEnabled());

        shader.setVec3(std::format("{}.position", base), position);
        shader.setVec3(std::format("{}.direction", base), calculateLightDirection(position, target));

        shader.setVec3(std::format("{}.ambient", base), ambient * intensity);
        shader.setVec3(std::format("{}.diffuse", base), diffuse * intensity);
        shader.setVec3(std::format("{}.specular", base), specular);

        shader.setFloat(std::format("{}.constant", base), 1.0f);
        shader.setFloat(std::format("{}.linear", base), 0.09f);
        shader.setFloat(std::format("{}.quadratic", base), 0.032f); // tweak shadow intensity

        // clamp
        shader.setFloat(std::format("{}.cutOff", base), glm::cos(glm::radians(cutoff)));
        shader.setFloat(std::format("{}.outerCutOff", base), glm::cos(glm::radians(outerCutoff)));
    }

    

    auto* singleton = engine::Singleton::getInstance();
    assert(singleton != nullptr && "Singleton not initialized !");
    SceneSettings& sceneSettings = singleton->sceneSettings();

    if (sceneSettings.drawLightsVisualHelpers)
    {
        glm::vec3 direction = glm::normalize(target - position);
        glm::vec3 defaultAxis = glm::vec3(0.0f, -1.0f, 0.0f); // cone points down

        // Compute quaternion rotation between default axis and desired direction
        glm::quat rotationQuat = glm::rotation(defaultAxis, direction);

        // Convert to rotation matrix
        glm::mat4 rotationMatrix = glm::toMat4(rotationQuat);

        // Compose final model matrix
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model *= rotationMatrix;
        //model = glm::scale(model, glm::vec3(0.25f, glm::length(target - position), 0.25f)); // scale lengthwise toward target
        model = glm::scale(model, glm::vec3(0.25f));

        // Pass model matrix to shader
        m_lightDebugShader.use();
        m_lightDebugShader.setMat4("projection", projection);
        m_lightDebugShader.setMat4("view", view);
        m_lightDebugShader.setVec4("customColor", m_debug_cone.getMaterial()->getAmbientColor());

        // You can pass glm::vec3(0) for rotation since model is already transformed
        auto localTransform = Transform(position, glm::vec3(0.0f), glm::vec3(1.0f));
        m_debug_cone.draw(m_lightDebugShader, projection, view, model, localTransform);
    }
}

void engine::SpotLight::clean()
{
    m_debug_cone.clean();
}