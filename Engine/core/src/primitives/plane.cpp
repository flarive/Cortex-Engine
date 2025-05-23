#include "../../include/primitives/plane.h"

#include "../../include/vertex.h"
#include "../../include/uvmapping.h"
#include "../../include/primitives/primitive.h"
#include "../../include/tools/helpers.h"

namespace engine
{
    inline std::vector<Vertex> generatePlaneVertices(float uvScale)
    {
        std::vector<engine::Vertex> vertices;

        // positions
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        glm::vec3 pos4(1.0f, 1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, uvScale);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(uvScale, 0.0f);
        glm::vec2 uv4(uvScale, uvScale);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // Calculate tangents and bitangents for both triangles
        glm::vec3 edge1, edge2;
        glm::vec2 deltaUV1, deltaUV2;
        float f;
        glm::vec3 tangent1, bitangent1, tangent2, bitangent2;

        // Triangle 1 (pos1, pos2, pos3)
        edge1 = pos2 - pos1;
        edge2 = pos3 - pos1;
        deltaUV1 = uv2 - uv1;
        deltaUV2 = uv3 - uv1;
        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // Triangle 2 (pos1, pos3, pos4)
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;
        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);

        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);

        // Fill vertex data for both triangles
        vertices.push_back({ pos1, nm, uv1, tangent1, bitangent1 });
        vertices.push_back({ pos2, nm, uv2, tangent1, bitangent1 });
        vertices.push_back({ pos3, nm, uv3, tangent1, bitangent1 });

        vertices.push_back({ pos1, nm, uv1, tangent2, bitangent2 });
        vertices.push_back({ pos3, nm, uv3, tangent2, bitangent2 });
        vertices.push_back({ pos4, nm, uv4, tangent2, bitangent2 });

        return vertices;
    }



    void engine::Plane::setup(const std::shared_ptr<Material>& material)
    {
        m_material = material; // Store material reference

        const UvMapping uv{};
        setup(material, uv);
    }

    void engine::Plane::setup(const std::shared_ptr<Material>& material, const UvMapping& uv)
    {
        m_material = material;
        m_uvScale = uv.getUvScale();

        setup(); // Geometry setup

        if (material)
            material->loadTexturesAsync(); // Let material handle texture loading
    }

    void engine::Plane::setup()
    {
        // positions
        //glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        //glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        //glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        //glm::vec3 pos4(1.0f, 1.0f, 0.0f);
        //// texture coordinates
        //glm::vec2 uv1(0.0f, m_uvScale);
        //glm::vec2 uv2(0.0f, 0.0f);
        //glm::vec2 uv3(m_uvScale, 0.0f);
        //glm::vec2 uv4(m_uvScale, m_uvScale);
        //// normal vector
        //glm::vec3 nm(0.0f, 0.0f, 1.0f);

        //// calculate tangent/bitangent vectors of both triangles
        //glm::vec3 tangent1, bitangent1;
        //glm::vec3 tangent2, bitangent2;
        //// triangle 1
        //// ----------
        //glm::vec3 edge1 = pos2 - pos1;
        //glm::vec3 edge2 = pos3 - pos1;
        //glm::vec2 deltaUV1 = uv2 - uv1;
        //glm::vec2 deltaUV2 = uv3 - uv1;

        //float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        //tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        //tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        //tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        //bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        //bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        //bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        //// triangle 2
        //// ----------
        //edge1 = pos3 - pos1;
        //edge2 = pos4 - pos1;
        //deltaUV1 = uv3 - uv1;
        //deltaUV2 = uv4 - uv1;

        //f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        //tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        //tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        //tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


        //bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        //bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        //bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


        //float quadVertices[] = {
        //    // positions            // normal         // texcoords  // tangent                          // bitangent
        //    pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        //    pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        //    pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

        //    pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        //    pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        //    pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        //};

        std::vector<Vertex> vertices = generatePlaneVertices(m_uvScale);


        // configure plane VAO
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        
        //glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
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
    }

    std::vector<engine::Vertex> engine::Plane::generateVertices()
    {
        return generatePlaneVertices(m_uvScale);
    }


    // draws the model, and thus all its meshes
    void engine::Plane::draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation)
    {
        shader.use();

        if (m_material)
        {
            m_material->bind(shader);
            shader.setVec3("material.ambient_color", m_material->getAmbientColor());
            shader.setFloat("material.ambient_intensity", m_material->getAmbientIntensity());

            shader.setFloat("material.heightScale", m_material->getHeightIntensity());
            shader.setFloat("material.normalMapIntensity", m_material->getNormalIntensity());
            shader.setFloat("material.emissiveIntensity", 0.0f);
        }

        auto normalizedRotation = engine::Helpers::normalizeRotation(rotation);

        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(normalizedRotation.angle), normalizedRotation.axis);
        model = glm::scale(model, glm::vec3(size.x, size.y, size.z));
        shader.setMat4("model", model);
        shader.setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        shader.setBool("hasTangents", true);

        // Render plane
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        m_material->unbind(); // Unbind textures to prevent OpenGL state retention
    }
}