#pragma once

#include "../shader.h"
#include "../primitive.h"

class DirectionalLight
{
public:
    DirectionalLight()
    {
        setupDirectionalLight();
    }

    

    // draws the model, and thus all its meshes
    void Draw(const Shader& shader, const glm::mat4& _projection, const glm::mat4& _view, const glm::vec3& position)
    {
        // directional light
        shader.setBool("dirLight.use", true);
        shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);


 
        // also draw the lamp object(s)
        lightCubeShader.use();

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(VAO);

        lightCubeShader.setMat4("projection", _projection);
        lightCubeShader.setMat4("view", _view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        

        glBindVertexArray(0);
    }

    void clean()
    {
        glDeleteVertexArrays(1, &VAO);
    }

private:

    // render data 
    unsigned int VBO, VAO;

    const float* vertices = primitive::getCubeVertices();

    Shader lightCubeShader;


    void setupDirectionalLight()
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
};
