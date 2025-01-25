#pragma once

#include "light.h"

class SpotLight : public Light
{
public:
    SpotLight() : Light(0)
    {
        setupSpotLight();
    }

    SpotLight(unsigned int index) : Light(index)
    {
        setupSpotLight();
    }

    

    // draws the model, and thus all its meshes
    void draw(const Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& position, const glm::vec3& target) override
    {
        shader.setBool("spotLight.use", true);

        shader.setVec3("spotLight.position", position);
        shader.setVec3("spotLight.direction", calculateSpotlightDirection(position, target));
        shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

        shader.setFloat("spotLight.constant", 1.0f);
        shader.setFloat("spotLight.linear", 0.09f);
        shader.setFloat("spotLight.quadratic", 0.032f);

        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

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

    void clean() override
    {
        glDeleteVertexArrays(1, &VAO);
    }

private:

    const float* vertices = primitive::getCubeVertices();

    void setupSpotLight()
    {
        glGenVertexArrays(1, &VAO);  // 1 is the uniqueID of the VAO
        glGenBuffers(1, &VBO);  // 1 is the uniqueID of the VBO

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), vertices, GL_STATIC_DRAW);

        GLsizei stride = 8;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // load light cube debug shader
        lightCubeShader.init("shaders/light_cube.vertex", "shaders/light_cube.frag");
    }

    glm::vec3 calculateSpotlightDirection(const glm::vec3& position, const glm::vec3& target)
    {
        return glm::normalize(target - position);
    }
};
