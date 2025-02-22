#include "../../include/primitives/sphere.h"

#include "../../include/texture.h"
#include "../../include/uvmapping.h"
#include "../../include/materials/material.h"

void engine::Sphere::setup(const glm::uvec3& color)
{
    m_diffuseMap = engine::Texture::createSolidColorTexture(color.r, color.g, color.b, 255);
    setup();
}

void engine::Sphere::setup(const engine::Material& material)
{
    setup(material, UvMapping{});
}

void engine::Sphere::setup(const engine::Material& material, const UvMapping& uv)
{
    m_uvScale = uv.getUvScale();
    m_ambientColor = material.getAmbientColor();

    loadTextures(material);
    setup();
}

void engine::Sphere::setup()
{
    // Generate VAO and buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    constexpr unsigned int X_SEGMENTS = 64;
    constexpr unsigned int Y_SEGMENTS = 64;
    constexpr float PI = 3.14159265359f;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    // Preallocate memory for better performance
    const size_t numVertices = (X_SEGMENTS + 1) * (Y_SEGMENTS + 1);
    positions.reserve(numVertices);
    uv.reserve(numVertices);
    normals.reserve(numVertices);
    indices.reserve(Y_SEGMENTS * (X_SEGMENTS + 1) * 2);

    // Generate sphere vertices
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
    {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            float xSegment = static_cast<float>(x) / static_cast<float>(X_SEGMENTS);
            float ySegment = static_cast<float>(y) / static_cast<float>(Y_SEGMENTS);
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.emplace_back(xPos, yPos, zPos);
            uv.emplace_back(xSegment * m_uvScale, ySegment * m_uvScale);
            normals.emplace_back(xPos, yPos, zPos);
        }
    }

    // Generate indices using triangle strip order
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

    // Interleave data for OpenGL buffer
    std::vector<float> data;
    data.reserve(numVertices * 8); // 3 (pos) + 3 (normal) + 2 (uv)

    for (size_t i = 0; i < numVertices; ++i)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
    }

    // Upload data to OpenGL
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Define vertex attributes
    constexpr unsigned int stride = (3 + 3 + 2) * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void engine::Sphere::loadTextures(const engine::Material& material)
{
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
    if (material.hasHeightMap())
        m_heightMap = engine::Texture::loadTexture(material.getHeightTexPath(), true, false);
}


// draws the model, and thus all its meshes
void engine::Sphere::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle, const glm::vec3& rotationAxis)
{
    shader.use();

    std::vector<unsigned int> textures = {
    m_diffuseMap, m_specularMap, m_normalMap,
    m_metallicMap, m_roughnessMap, m_aoMap, m_heightMap
    };

    for (unsigned int i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
    

    if (shader.name == "pbr")
    {
        
    }
    else if (shader.name == "blinnphong") // blinn phong shader
    {
        shader.use();
        shader.setVec3("material.ambient_color", 0.01f, 0.01f, 0.01f);
        //shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.setInt("material.texture_diffuse1", 0); // texture 0
        shader.setInt("material.texture_specular1", 1); // texture 1
        shader.setInt("material.texture_normal1", 2); // texture 2
        //shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setBool("material.has_texture_normal_map", m_normalMap != 0);
        shader.setFloat("uvScale", m_uvScale);
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