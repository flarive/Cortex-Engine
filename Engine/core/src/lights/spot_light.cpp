#include "../../include/lights/spot_light.h"

#include <format>

engine::SpotLight::SpotLight() : Light(0)
{
}

engine::SpotLight::SpotLight(unsigned int index) : Light(index)
{
}

void engine::SpotLight::setup(const Color& ambient, const glm::vec3& position, const glm::vec3& target)
{
    m_ambientColor = ambient;

    m_lightPosition = position;
    m_lightTarget = target;
    
    glGenVertexArrays(1, &VAO);  // 1 is the uniqueID of the VAO
    glGenBuffers(1, &VBO);  // 1 is the uniqueID of the VBO

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

    GLsizei stride = 8;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load light cube debug shader
    lightCubeShader.init("light_cube", "shaders/debug/debug_light.vertex", "shaders/debug/debug_light.frag");
}


// draws the model, and thus all its meshes
void engine::SpotLight::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, float intensity, const glm::vec3& position, const glm::vec3& target)
{
    m_lightPosition = position;
    m_lightTarget = target;
    
    std::string base = std::format("spotLights[{}]", m_index);

    shader.setBool(std::format("{}.use", base), true);

    shader.setVec3(std::format("{}.position", base), position);
    shader.setVec3(std::format("{}.direction", base), calculateLightDirection(position, target));
    shader.setVec3(std::format("{}.ambient", base), m_ambientColor);
    shader.setVec3(std::format("{}.diffuse", base), intensity * 1.0f, intensity * 1.0f, intensity * 1.0f);
    shader.setVec3(std::format("{}.specular", base), 1.0f, 1.0f, 1.0f);

    shader.setFloat(std::format("{}.constant", base), 1.0f);
    shader.setFloat(std::format("{}.linear", base), 0.09f);
    shader.setFloat(std::format("{}.quadratic", base), 0.032f);

    shader.setFloat(std::format("{}.cutOff", base), glm::cos(glm::radians(m_cutoff)));
    shader.setFloat(std::format("{}.outerCutOff", base), glm::cos(glm::radians(m_outerCutoff)));

    if (DISPLAY_DEBUG_LIGHT_CUBE)
    {
        // also draw the lamp object(s)
        lightCubeShader.use();

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(VAO);

        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, glm::vec3(LIGHT_CUBE_SIZE)); // Make it a smaller cube
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
    }
}


void engine::SpotLight::setCutOff(float cutoff)
{
    m_cutoff = cutoff;
}

void engine::SpotLight::setOuterCutOff(float outerCutoff)
{
    m_outerCutoff = outerCutoff;
}

void engine::SpotLight::clean()
{
    glDeleteVertexArrays(1, &VAO);
}