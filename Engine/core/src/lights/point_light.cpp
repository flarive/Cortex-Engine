#include "../../include/lights/point_light.h"


engine::PointLight::PointLight() : Light(0)
{
}

engine::PointLight::PointLight(unsigned int index) : Light(index)
{
    setup();
}

void engine::PointLight::setup()
{
    // load light cube debug shader
    m_lightDebugShader.init("light_debug", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");

    auto matDebugLight = std::make_shared<engine::Material>(engine::Color(1.0f, 0.0f, 0.0f, 1.0f));
    m_sphere.setup(matDebugLight);
}


void engine::PointLight::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, float intensity, const glm::vec3& position, const glm::vec3& target, const glm::vec3& size, const glm::vec3& rotation)
{
    std::string base = std::format("pointLights[{}]", m_index);

    shader.use();
    shader.setBool(std::format("{}.use", base), true);

    shader.setVec3(std::format("{}.position", base), position);
    shader.setVec3(std::format("{}.ambient", base), ambient);
    shader.setVec3(std::format("{}.diffuse", base), intensity * 1.0f, intensity * 1.0f, intensity * 1.0f);
    shader.setVec3(std::format("{}.specular", base), 1.0f, 1.0f, 1.0f);

    shader.setFloat(std::format("{}.constant", base), 1.0f);
    shader.setFloat(std::format("{}.linear", base), 0.09f);
    shader.setFloat(std::format("{}.quadratic", base), 0.032f);


    if (DISPLAY_DEBUG_LIGHT_CUBE)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, glm::vec3(LIGHT_CUBE_SIZE)); // Make it a smaller cube

        // also draw the lamp object(s)
        m_lightDebugShader.use();
        m_lightDebugShader.setMat4("projection", projection);
        m_lightDebugShader.setMat4("view", view);
        m_lightDebugShader.setMat4("model", model);

        m_sphere.draw(m_lightDebugShader, position, glm::vec3(0.05f));
    }
}

void engine::PointLight::clean()
{
    glDeleteVertexArrays(1, &VAO);
}