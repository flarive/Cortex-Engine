#include "../../include/lights/directional_light.h"

#include "../../include/tools/helpers.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>  // For glm::rotation and glm::eulerAngles
#include <glm/gtx/transform.hpp>   // Optional: glm::translate, rotate, scale

#include <format>


engine::DirectionalLight::DirectionalLight() : Light(0)
{
}

engine::DirectionalLight::DirectionalLight(unsigned int index) : Light(index)
{
    setup();
}

void engine::DirectionalLight::setup()
{
    // load light cube debug shader
    m_lightDebugShader.init("light_cube", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");


    auto matDebugLight = std::make_shared<engine::Material>(engine::Color(1.0f, 0.0f, 0.0f, 0.2f));
    m_debug_cylinder.setup(matDebugLight);

}

void engine::DirectionalLight::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, float intensity, const glm::vec3& position, const glm::vec3& target, const glm::vec3& size, const glm::vec3& rotation)
{
    std::string base = std::format("dirLights[{}]", m_index);

    // directional light
    shader.setBool(std::format("{}.use", base), true);

    shader.setVec3(std::format("{}.ambient", base), ambient);
    shader.setVec3(std::format("{}.diffuse", base), intensity * 1.0f, intensity * 1.0f, intensity * 1.0f);
    shader.setVec3(std::format("{}.specular", base), 1.0f, 1.0f, 1.0f);

    shader.setVec3(std::format("{}.direction", base), calculateLightDirection(position, target));


    if (DISPLAY_DEBUG_LIGHT)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        auto normalizedRotation = engine::Helpers::normalizeRotation(rotation);
        //model = glm::rotate(model, glm::radians(normalizedRotation.angle), normalizedRotation.axis);
        model = glm::scale(model, glm::vec3(LIGHT_CUBE_SIZE)); // Make it a smaller cube


        glm::vec3 direction = glm::normalize(target - position);

        // Compute rotation to align the cylinder's Y-axis with the direction
        glm::quat rotationQuat = glm::rotation(glm::vec3(0.0f, 1.0f, 0.0f), direction);
        glm::vec3 eulerAngles = glm::eulerAngles(rotationQuat); // In radians
        glm::vec3 eulerDegrees = glm::degrees(eulerAngles);     // Now in degrees



        // also draw the lamp object(s)
        m_lightDebugShader.use();
        m_lightDebugShader.setMat4("projection", projection);
        m_lightDebugShader.setMat4("view", view);
        m_lightDebugShader.setMat4("model", model);
        m_lightDebugShader.setVec4("customColor", m_debug_cylinder.getMaterial()->getAmbientColor()); // RGBA

        m_debug_cylinder.draw(m_lightDebugShader, position, glm::vec3(0.05f), eulerDegrees);
    }





}

void engine::DirectionalLight::clean()
{
    //glDeleteVertexArrays(1, &VAO);
}