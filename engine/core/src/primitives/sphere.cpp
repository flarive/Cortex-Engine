#include "../../include/primitives/sphere.h"

#include "../../include/vertex.h"
#include "../../include/tools/helpers.h"

void engine::Sphere::setup(const std::shared_ptr<Material>& material)
{
    m_material = material;
    const UvMapping uv{};
    setup(material, uv);
}

void engine::Sphere::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
{
    m_material = material;
    m_uvScale = uv.getUvScale();
    setup();
    if (material)
        material->loadTexturesAsync();
}

void engine::Sphere::setup()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    constexpr unsigned int X_SEGMENTS = 64;
    constexpr unsigned int Y_SEGMENTS = 64;
    constexpr float PI = 3.14159265359f;

    std::vector<glm::vec3> positions, normals, tangents, bitangents;
    std::vector<glm::vec2> uv;
    std::vector<unsigned int> indices;

    const size_t numVertices = (X_SEGMENTS + 1) * (Y_SEGMENTS + 1);
    positions.reserve(numVertices);
    uv.reserve(numVertices);
    normals.reserve(numVertices);
    indices.reserve(Y_SEGMENTS * (X_SEGMENTS + 1) * 2);

    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = static_cast<float>(x) / static_cast<float>(X_SEGMENTS);
            float ySegment = static_cast<float>(y) / static_cast<float>(Y_SEGMENTS);
            float xPos = cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
            float yPos = cos(ySegment * PI);
            float zPos = sin(xSegment * 2.0f * PI) * sin(ySegment * PI);

            glm::vec3 position = { xPos, yPos, zPos };
            glm::vec2 texCoord = { xSegment * m_uvScale, ySegment * m_uvScale };
            glm::vec3 normal = glm::normalize(position);

            positions.emplace_back(position);
            uv.emplace_back(xSegment * m_uvScale, ySegment * m_uvScale);
            normals.emplace_back(normal);

            glm::vec3 tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), normal));
            glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

            tangents.push_back(tangent);
            bitangents.push_back(bitangent);
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    data.reserve(numVertices * 14); // 3 (pos) + 3 (normal) + 2 (uv) + 3 (tangent) + 3 (bitangent)

    for (size_t i = 0; i < numVertices; ++i)
    {
        data.insert(data.end(), { positions[i].x, positions[i].y, positions[i].z });
        data.insert(data.end(), { normals[i].x, normals[i].y, normals[i].z });
        data.insert(data.end(), { uv[i].x, uv[i].y });
        data.insert(data.end(), { tangents[i].x, tangents[i].y, tangents[i].z });
        data.insert(data.end(), { bitangents[i].x, bitangents[i].y, bitangents[i].z });
    }

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    constexpr unsigned int stride = 14 * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)(11 * sizeof(float)));

    glBindVertexArray(0);
}

std::vector<engine::Vertex> engine::Sphere::generateVertices()
{
    std::vector<Vertex> vertices{};

    return vertices;
}

void engine::Sphere::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
{
    shader.use();
    if (m_material)
    {
        m_material->bind(shader);
        shader.setVec3("material.ambient_color", m_material->getAmbientColor());
        shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());
        shader.setBool("hasTangents", true);

        shader.setFloat("material.heightScale", m_material->getHeightIntensity());
        shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
        shader.setFloat("material.emissiveIntensity", 0.0f);
    }

    auto normalizedRotation = engine::Helpers::normalizeRotation(rotation);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, glm::radians(normalizedRotation.angle), normalizedRotation.axis);
    model = glm::scale(model, size);
    shader.setMat4("model", model);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_material->unbind();
}
