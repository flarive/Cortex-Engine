#pragma once

#include "shader.h"
#include "primitive.h"

class Cubes
{
public:
    Cubes(unsigned int quantity)
    {
        setupCubes(quantity);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
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
        //shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);



        // move outside !!!!!!!!!!!!!!!!!!!!!
        glm::vec3 cubePositions[] =
        {
            glm::vec3(0.0f,  0.0f,  0.0f),
            glm::vec3(2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f,  2.0f, -2.5f),
            glm::vec3(1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };

        shader.use();

        // render the cubes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
    }

private:
    
    // render data 
    unsigned int VBO, VAO;

    unsigned int diffuseMap = 0;
    unsigned int specularMap = 0;


    // Set up vertex data for a 3d cube (position and uvs)
    const float* vertices = primitive::getCubeVertices();



    

    void setupCubes(unsigned int quantity)
    {
        glGenVertexArrays(1, &VAO);  // 1 is the uniqueID of the VAO
        glGenBuffers(1, &VBO);  // 1 is the uniqueID of the VBO

        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), vertices, GL_STATIC_DRAW);

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
        diffuseMap = texture_helper::soil_load_texture("container2.png", false, true);
        specularMap = texture_helper::soil_load_texture("container2_specular.png", true, false);
    }
};