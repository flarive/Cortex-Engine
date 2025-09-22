#include "../../include/primitives/cube.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/materials/material.h"
#include "../../include/tools/helpers.h"

#include <vector>
#include <glm/glm.hpp>


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

    if (material && material->hasDiffuseMap())
        material->loadTexturesAsync(); // Let material handle texture loading
}

void engine::Cube::setup()
{
    std::vector<Vertex> vertices = generateCubeVertices();

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

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
    return generateCubeVertices();
}

//void engine::Cube::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
//{
//    shader.use();
//
//    if (m_material) {
//        m_material->bind(shader);
//        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
//        shader.setVec3("material.diffuse_color", m_material->getDiffuseColor());
//        shader.setVec3("material.specular_color", m_material->getSpecularColor());
//
//        shader.setFloat("material.shininess", m_material->getShininessIntensity());
//
//        shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
//        
//
//        shader.setFloat("material.heightScale", m_material->getHeightIntensity());
//        shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
//        shader.setFloat("material.emissiveIntensity", m_material->getEmissiveIntensity());
//    }
//
//    auto normalizedRotation = engine::Helpers::normalizeRotation(rotation);
//
//    glm::mat4 model = glm::mat4(1.0f);
//    model = glm::translate(model, position);
//    if (normalizedRotation.angle != 0) model = glm::rotate(model, glm::radians(normalizedRotation.angle), normalizedRotation.axis);
//    model = glm::scale(model, size);
//    shader.setMat4("model", model);
//    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
//    shader.setBool("hasTangents", true);
//
//    glBindVertexArray(m_VAO);
//    glDrawArrays(GL_TRIANGLES, 0, 36);
//    glBindVertexArray(0);
//
//    m_material->unbind();
//}

void engine::Cube::draw(Shader& shader, const glm::mat4 transformMatrix, Transform& localTransform)
{
    shader.use();

    m_position = localTransform.getLocalPosition();
    m_rotation = localTransform.getLocalRotation();
    m_scale = localTransform.getLocalScale();

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

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    m_material->unbind();
}