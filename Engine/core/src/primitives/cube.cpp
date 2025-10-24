#include "../../include/primitives/cube.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/materials/material.h"
#include "../../include/tools/helpers.h"

#include <vector>
#include <glm/glm.hpp>

engine::Cube::Cube(const glm::vec3& _position)
    : Primitive(_position)
{
}

engine::Cube::Cube(const float& _size, const glm::vec3& _position)
    : Primitive(_position), m_width(_size), m_height(_size), m_depth(_size)
{
}

engine::Cube::Cube(const float& _width, const float& _height, const float& _depth, const glm::vec3& _position)
    : Primitive(_position), m_width(_width), m_height(_height), m_depth(_depth)
{
}

void engine::Cube::setup()
{
    geometrySetup(); // Geometry setup
}

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

    geometrySetup(); // Geometry setup

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync(); // Let material handle texture loading
}

void engine::Cube::geometrySetup()
{
    std::vector<Vertex> vertices = generateVertices();

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // Send to GPU
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Tangent attribute
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(3);

    // Bitangent attribute
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
}

std::vector<engine::Vertex> engine::Cube::generateVertices()
{
    return generateCuboidVertices(m_width, m_height, m_depth, m_uvScale);
}

void engine::Cube::draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform)
{
    shader.use();

    position = localTransform.getLocalPosition();
    rotation = localTransform.getLocalRotation();
    scale = localTransform.getLocalScale();

    if (m_material)
    {
        m_material->bind(shader);
        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
        shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
        shader.setVec3("material.specular_color", m_material->getSpecularColor());

        shader.setFloat("material.shininess", m_material->getShininessIntensity());

        shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());


        shader.setFloat("material.heightScale", m_material->getHeightIntensity());
        shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
        shader.setFloat("material.emissiveIntensity", m_material->getEmissiveIntensity());
    }

    shader.setMat4("model", transformMatrix);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transformMatrix))));
    shader.setBool("hasTangents", true);

    // Send to GPU
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    m_material->unbind();
}

void engine::Cube::clean()
{

}