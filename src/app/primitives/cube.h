#pragma once

#include "primitive.h"

#include "../shader.h"
#include "../texture_helper.h"

class Cube : public Primitive
{
public:
    Cube()
    {
        setupCube();
    }

    // draws the model, and thus all its meshes
    void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle = 0.0f, const glm::vec3& rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f))
    {
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        shader.use();
        shader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
        //shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.setInt("material.texture_diffuse1", 0); // texture 0
        shader.setInt("material.texture_specular1", 1); // texture 1
        shader.setBool("material.has_normal_map", normalMap > 0);
        //shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);


        shader.use();

        // render the cubes
        glBindVertexArray(VAO);

        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, position);
        if (rotationAngle != 0) model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
        model = glm::scale(model, size);
        shader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
    }

private:

    void setupCube()
    {
        glGenVertexArrays(1, &VAO);  // 1 is the uniqueID of the VAO
        glGenBuffers(1, &VBO);  // 1 is the uniqueID of the VBO

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

        GLsizei stride = 8;

        // position attribute (XYZ)
        // layout (location = 0), vec3, vector of floats, normalized, stride, offset in buffer
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0); // stride 0 to 2

        // normal attribute (XYZ)
        // layout(location = 1), vec3, vector of floats, normalized, stride, offset in buffer
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1); // stride 3 to 5

        // texture coord attribute (RGB)
        // layout(location = 2), vec3, vector of floats, normalized, stride, offset in buffer
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2); // stride 6 to 7

        

        // load texture
        diffuseMap = texture_helper::soil_load_texture("textures/container2.png", true);
        specularMap = texture_helper::soil_load_texture("textures/container2_specular.png", false);
    }
};
