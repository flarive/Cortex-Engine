#pragma once

#include "../common_defines.h"
#include "../vertex.h"
#include "../shader.h"
#include "../uvmapping.h"
#include "../materials/material.h"
#include "../misc/noncopyable.h"

namespace engine
{
    /// <summary>
    /// Abstract class for primitives
    /// </summary>
    class Primitive
    {
    protected:
        unsigned int m_VBO{}, m_VAO{}, m_EBO{};

        std::shared_ptr<Material> m_material{};

        Color m_ambientColor{};

        unsigned int m_cubemapTexture{};

        float m_uvScale{ 1.0f };

    public:
        Primitive() = default;
        virtual ~Primitive() = default;

        virtual void setup(const std::shared_ptr<Material>& material) = 0;
        virtual void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) = 0;

        virtual void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f)) = 0;

        // optional: de-allocate all resources once they've outlived their purpose
        virtual void clean();

        virtual std::vector<Vertex> generateVertices() = 0;

        std::shared_ptr<Material> getMaterial() { return m_material; }


        static float* getScaledPlaneVertices(float uvScale)
        {
            float* planeVertices = new float[84] {
            // Positions          // Normals       // TexCoords      // Tangents       // Bitangents
            1.0f, -0.01f, 1.0f, 0.0f, 1.0f, 0.0f, uvScale, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -0.01f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, uvScale, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -0.01f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

                1.0f, -0.01f, 1.0f, 0.0f, 1.0f, 0.0f, uvScale, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                1.0f, -0.01f, -1.0f, 0.0f, 1.0f, 0.0f, uvScale, uvScale, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -0.01f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, uvScale, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
            };

            return planeVertices; // Caller must delete[] this
        }


        static float* GetScaledQuadVertices(float uvScale)
        {
            float* quadVertices = new float[48] {
            // positions            // normals         // texcoords
            1.0f, -0.01f,  1.0f,  0.0f, 1.0f, 0.0f,  uvScale, 0.0f,
            -1.0f, -0.01f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, uvScale,
            -1.0f, -0.01f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

             1.0f, -0.01f,  1.0f,  0.0f, 1.0f, 0.0f, uvScale, 0.0f,
             1.0f, -0.01f, -1.0f,  0.0f, 1.0f, 0.0f, uvScale, uvScale,
            -1.0f, -0.01f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, uvScale
            };

            return quadVertices; // Caller must delete[] this
        }


    private:
        virtual void setup() = 0;
    };


    //<summary>
    //Set up vertex data for a 3d cube (counter-countwise)
    //</summary>
    inline const float cubeVertices[] = {
        // positions          // normals           // texture coords

        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
        1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
    };


    inline std::vector<engine::Vertex> generateSphereVertices(float radius, float uvScale)
    {
        std::vector<engine::Vertex> vertices;

        constexpr unsigned int X_SEGMENTS = 64;
        constexpr unsigned int Y_SEGMENTS = 64;
        constexpr float PI = 3.14159265359f;

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
                glm::vec3 normal = glm::normalize(position);
                glm::vec2 texCoord = { xSegment * uvScale, ySegment * uvScale };

                // Calculate tangent and bitangent
                glm::vec3 tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), normal));
                glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

                engine::Vertex vertex;
                vertex.position = position;
                vertex.normal = normal;
                vertex.texCoords = texCoord;
                vertex.tangent = tangent;
                vertex.bitangent = bitangent;

                vertices.push_back(vertex);
            }
        }

        return vertices;
    }

    inline std::vector<Vertex> generateCylinderVertices(unsigned int sectorCount = 36, float height = 2.0f, float radius = 1.0f, float uvScale = 1.0f)
    {
        std::vector<Vertex> vertices;
        const float PI = 3.14159265359f;
        float halfHeight = height / 2.0f;

        // Side surface
        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float sectorAngle = 2 * PI * i / sectorCount;
            float x = cosf(sectorAngle);
            float z = sinf(sectorAngle);

            glm::vec3 normal(x, 0.0f, z);
            glm::vec2 texCoord((float)i / sectorCount * uvScale, 0.0f);

            // Bottom vertex
            vertices.push_back(Vertex{ glm::vec3(radius * x, -halfHeight, radius * z), normal, texCoord });

            // Top vertex
            texCoord.y = uvScale;
            vertices.push_back(Vertex{ glm::vec3(radius * x, halfHeight, radius * z), normal, texCoord });
        }

        // Top cap center
        glm::vec3 topCenter(0.0f, halfHeight, 0.0f);
        glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
        glm::vec2 topTex(0.5f * uvScale, 0.5f * uvScale);
        vertices.push_back(Vertex{ topCenter, topNormal, topTex });

        // Top cap ring
        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float angle = 2 * PI * i / sectorCount;
            float x = cosf(angle);
            float z = sinf(angle);
            glm::vec3 pos(radius * x, halfHeight, radius * z);
            glm::vec2 tex(0.5f * (x + 1.0f) * uvScale, 0.5f * (z + 1.0f) * uvScale);
            vertices.push_back(Vertex{ pos, topNormal, tex });
        }

        // Bottom cap center
        glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f);
        glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
        glm::vec2 bottomTex(0.5f * uvScale, 0.5f * uvScale);
        vertices.push_back(Vertex{ bottomCenter, bottomNormal, bottomTex });

        // Bottom cap ring
        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float angle = 2 * PI * i / sectorCount;
            float x = cosf(angle);
            float z = sinf(angle);
            glm::vec3 pos(radius * x, -halfHeight, radius * z);
            glm::vec2 tex(0.5f * (x + 1.0f) * uvScale, 0.5f * (z + 1.0f) * uvScale);
            vertices.push_back(Vertex{ pos, bottomNormal, tex });
        }

        return vertices;
    }

    inline std::vector<Vertex> generateConeVertices(unsigned int sectorCount = 36, float height = 2.0f, float radius = 1.0f, float uvScale = 1.0f)
    {
        const float halfHeight = height / 2.0f;
        const float PI = 3.14159265359f;

        std::vector<Vertex> vertices;

        // Tip of the cone
        glm::vec3 tip(0.0f, halfHeight, 0.0f);
        glm::vec3 tipNormal(0.0f, 1.0f, 0.0f);
        vertices.push_back(Vertex{ tip, tipNormal, glm::vec2(0.5f, 1.0f) });

        // Base ring
        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float angle = 2 * PI * i / sectorCount;
            float x = cosf(angle);
            float z = sinf(angle);
            glm::vec3 pos(radius * x, -halfHeight, radius * z);
            glm::vec3 normal = glm::normalize(glm::vec3(x, radius / height, z));
            glm::vec2 tex((x + 1.0f) * 0.5f * uvScale, (z + 1.0f) * 0.5f * uvScale);
            vertices.push_back(Vertex{ pos, normal, tex });
        }

        // Base center
        glm::vec3 baseCenter(0.0f, -halfHeight, 0.0f);
        glm::vec3 baseNormal(0.0f, -1.0f, 0.0f);
        vertices.push_back(Vertex{ baseCenter, baseNormal, glm::vec2(0.5f, 0.5f) });

        // Base ring again for cap
        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float angle = 2 * PI * i / sectorCount;
            float x = cosf(angle);
            float z = sinf(angle);
            glm::vec3 pos(radius * x, -halfHeight, radius * z);
            glm::vec2 tex((x + 1.0f) * 0.5f * uvScale, (z + 1.0f) * 0.5f * uvScale);
            vertices.push_back(Vertex{ pos, baseNormal, tex });
        }

        return vertices;
    }

    inline float screenQuadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    inline float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
}