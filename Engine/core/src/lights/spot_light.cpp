#include "../../include/lights/spot_light.h"

#include <format>

engine::SpotLight::SpotLight() : Light(0)
{
}

engine::SpotLight::SpotLight(unsigned int index) : Light(index)
{
    setup();
}

void engine::SpotLight::setup()
{
    //glGenVertexArrays(1, &VAO);  // 1 is the uniqueID of the VAO
    //glGenBuffers(1, &VBO);  // 1 is the uniqueID of the VBO

    //// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    //glBindVertexArray(VAO);

    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

    //GLsizei stride = 8;
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    // load light cube debug shader
    m_lightDebugShader.init("light_cube", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");

    auto matDebugLight = std::make_shared<engine::Material>(engine::Color(1.0f, 0.0f, 0.0f, 1.0f));
    m_debug_cone.setup(matDebugLight);
}

void engine::SpotLight::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const Color& ambient, float intensity, const glm::vec3& position, const glm::vec3& target, const glm::vec3& size, const glm::vec3& rotation)
{
    std::string base = std::format("spotLights[{}]", m_index);

    shader.setBool(std::format("{}.use", base), true);

    shader.setVec3(std::format("{}.position", base), position);
    shader.setVec3(std::format("{}.direction", base), calculateLightDirection(position, target));
    shader.setVec3(std::format("{}.ambient", base), ambient);
    shader.setVec3(std::format("{}.diffuse", base), intensity * 1.0f, intensity * 1.0f, intensity * 1.0f);
    shader.setVec3(std::format("{}.specular", base), 1.0f, 1.0f, 1.0f);

    shader.setFloat(std::format("{}.constant", base), 1.0f);
    shader.setFloat(std::format("{}.linear", base), 0.09f);
    shader.setFloat(std::format("{}.quadratic", base), 0.032f); // tweak shadow intensity

    // clamp
    shader.setFloat(std::format("{}.cutOff", base), glm::cos(glm::radians(cutoff)));
    shader.setFloat(std::format("{}.outerCutOff", base), glm::cos(glm::radians(outerCutoff)));

    if (DISPLAY_DEBUG_LIGHT)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, glm::vec3(LIGHT_CUBE_SIZE)); // Make it a smaller cube

        // also draw the lamp object(s)
        m_lightDebugShader.use();
        m_lightDebugShader.setMat4("projection", projection);
        m_lightDebugShader.setMat4("view", view);
        m_lightDebugShader.setMat4("model", model);
        m_lightDebugShader.setVec4("customColor", glm::vec4(1.0f, 0.5f, 0.2f, 1.0f)); // RGBA

        m_debug_cone.draw(m_lightDebugShader, position, glm::vec3(0.05f));
    }
}

void engine::SpotLight::clean()
{
    //glDeleteVertexArrays(1, &VAO);
}