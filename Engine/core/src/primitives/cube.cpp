#include "../../include/primitives/cube.h"

#include "../../include/texture.h"
#include "../../include/uvmapping.h"
#include "../../include/materials/material.h"

void engine::Cube::setup(const std::shared_ptr<Material>& material)
{
    m_material = material; // Store material reference

    const UvMapping uv{};
    setup(material, uv);
}

void engine::Cube::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    m_material = material;
    m_uvScale = uv.getUvScale();

    setup(); // Geometry setup

    if (material)
        material->loadTextures(); // Let material handle texture loading
}

void engine::Cube::setup()
{
    glGenVertexArrays(1, &m_VAO);  // 1 is the uniqueID of the VAO
    glGenBuffers(1, &m_VBO);  // 1 is the uniqueID of the VBO

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
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
}

// draws the model, and thus all its meshes
void engine::Cube::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle, const glm::vec3& rotationAxis)
{
    shader.use();

    if (shader.name == "cubemap")
    {
        // bind skybox map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);

        shader.setInt("skyboxTexture", 0); // texture 0
    }

    if (m_material)
    {
        m_material->bind(shader);
        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
        shader.setFloat("uvScale", m_uvScale);
        shader.setBool("hasTangents", false);
    }

    

    // calculate the model matrix for each object and pass it to shader before drawing
    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    model = glm::translate(model, position);
    if (rotationAngle != 0) model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
    model = glm::scale(model, size);
    shader.setMat4("model", model);

    // Render cube
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);

    m_material->unbind(); // Unbind textures to prevent OpenGL state retention
}