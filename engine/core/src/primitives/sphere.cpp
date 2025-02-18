#include "../../include/primitives/sphere.h"

#include "../../include/texture.h"
#include "../../include/uvmapping.h"
#include "../../include/materials/material.h"

void engine::Sphere::setup(const glm::uvec3& color)
{
    m_diffuseMap = engine::Texture::createSolidColorTexture(color.r, color.g, color.b, 255);
}

void engine::Sphere::setup(const engine::Material& material)
{
    const UvMapping uv{};
    setup(material, uv);
}

void engine::Sphere::setup(const engine::Material& material, const UvMapping& uv)
{
    m_uvScale = uv.getUvScale();

    setup();

    m_ambientColor = material.getAmbientColor();

    // load textures
    if (material.hasDiffuseMap())
        m_diffuseMap = engine::Texture::loadTexture(material.getDiffuseTexPath(), true, false);

    if (material.hasSpecularMap())
        m_specularMap = engine::Texture::loadTexture(material.getSpecularTexPath(), true, false);

    if (material.hasNormalMap())
        m_normalMap = engine::Texture::loadTexture(material.getNormalTexPath(), true, false);

    if (material.hasMetallicMap())
        m_metallicMap = engine::Texture::loadTexture(material.getMetallicTexPath(), true, false);

    if (material.hasRoughnessMap())
        m_roughnessMap = engine::Texture::loadTexture(material.getRoughnessTexPath(), true, false);

    if (material.hasAoMap())
        m_aoMap = engine::Texture::loadTexture(material.getAoTexPath(), true, false);
}

void engine::Sphere::setup()
{
    glGenVertexArrays(1, &m_VAO);

    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    std::vector<glm::vec3> positions{};
    std::vector<glm::vec2> uv{};
    std::vector<glm::vec3> normals{};
    std::vector<unsigned int> indices{};

    const unsigned int X_SEGMENTS{ 64 };
    const unsigned int Y_SEGMENTS{ 64 };
    const float PI{ 3.14159265359f };
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment{ (float)x / (float)X_SEGMENTS };
            float ySegment{ (float)y / (float)Y_SEGMENTS };
            float xPos{ std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI) };
            float yPos{ std::cos(ySegment * PI) };
            float zPos{ std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI) };

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow{ false };
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
    {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
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

    std::vector<float> data{};
    for (unsigned int i = 0; i < positions.size(); ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0)
        {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0)
        {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    unsigned int stride{ (3 + 2 + 3) * sizeof(float) };
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
}

// draws the model, and thus all its meshes
void engine::Sphere::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle, const glm::vec3& rotationAxis)
{
    if (shader.name == "pbr")
    {
        // bind diffuse (albedo) map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);

        // bind normal map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_normalMap);

        // bind metallic map
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_metallicMap);

        // bind roughness map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_roughnessMap);

        // bind ambient occlusion map
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, m_aoMap);
    }
    else if (shader.name == "blinnphong") // blinn phong shader
    {
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);

        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specularMap);

        // bind normal map
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_normalMap);

        shader.use();
        shader.setVec3("material.ambient_color", 0.01f, 0.01f, 0.01f);
        //shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.setInt("material.texture_diffuse1", 0); // texture 0
        shader.setInt("material.texture_specular1", 1); // texture 1
        shader.setInt("material.texture_normal1", 2); // texture 2
        //shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setBool("material.has_normal_map", m_normalMap > 0);
        shader.setFloat("uvScale", 2.0f);
    }
    
    

    // calculate the model matrix for each object and pass it to shader before drawing
    glm::mat4 model{ glm::mat4(1.0f) }; // make sure to initialize matrix to identity matrix first
    model = glm::translate(model, position);
    if (rotationAngle != 0) model = glm::rotate(model, glm::radians(rotationAngle), rotationAxis);
    model = glm::scale(model, size);
    shader.setMat4("model", model);
    shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

    // render the sphere
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}