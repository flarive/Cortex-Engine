#pragma once

#include "../common_defines.h"
#include "../vertex.h"
#include "../shader.h"
#include "../uvmapping.h"
#include "../materials/material.h"
#include "../misc/noncopyable.h"


#include "../transform.h"


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
        
        glm::vec3 position{};
        glm::vec3 rotation{};
        glm::vec3 scale{};
        
        Primitive() = default;
        virtual ~Primitive() = default;

        virtual void setup(const std::shared_ptr<Material>& material) = 0;
        virtual void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) = 0;

        virtual void draw(Shader& shader, const glm::mat4 transformMatrix, Transform& localTransform) = 0;

        // optional: de-allocate all resources once they've outlived their purpose
        virtual void clean();

        virtual std::vector<Vertex> generateVertices() = 0;

        std::shared_ptr<Material> getMaterial() { return m_material; }


        //glm::vec3 getLocalPosition() { return m_position; }
        //glm::vec3 getLocalScale() { return m_scale; }
        //glm::vec3 getLocalRotation() { return m_rotation; }


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


    inline std::vector<Vertex> generatePlaneVertices(float uvScale)
    {
        std::vector<engine::Vertex> vertices;

        // Define positions (XY plane, facing +Z)
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f); // Top-left
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f); // Bottom-left
        glm::vec3 pos3(1.0f, -1.0f, 0.0f); // Bottom-right
        glm::vec3 pos4(1.0f, 1.0f, 0.0f); // Top-right

        // Texture coordinates
        glm::vec2 uv1(0.0f, 0.0f);          // Top-left (now at v=0)
        glm::vec2 uv2(0.0f, uvScale);       // Bottom-left (now at v=uvScale)
        glm::vec2 uv3(uvScale, uvScale);    // Bottom-right
        glm::vec2 uv4(uvScale, 0.0f);       // Top-right

        
        //glm::vec2 uv1(0.0f, uvScale);
        //glm::vec2 uv2(0.0f, 0.0f);
        //glm::vec2 uv3(uvScale, 0.0f);
        //glm::vec2 uv4(uvScale, uvScale);

        // Normal vector (facing +Z)
        glm::vec3 normal(0.0f, 0.0f, 1.0f);

        // Triangle 1: pos1, pos3, pos2 (CCW)
        glm::vec3 edge1 = pos3 - pos1;
        glm::vec3 edge2 = pos2 - pos1;
        glm::vec2 deltaUV1 = uv3 - uv1;
        glm::vec2 deltaUV2 = uv2 - uv1;
        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent1, bitangent1;
        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // Triangle 2: pos1, pos4, pos3 (CCW)
        edge1 = pos4 - pos1;
        edge2 = pos3 - pos1;
        deltaUV1 = uv4 - uv1;
        deltaUV2 = uv3 - uv1;
        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent2, bitangent2;
        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);

        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);

        // Add vertices with CCW winding
        vertices.emplace_back(pos1, normal, uv1, tangent1, bitangent1);
        vertices.emplace_back(pos3, normal, uv3, tangent1, bitangent1);
        vertices.emplace_back(pos2, normal, uv2, tangent1, bitangent1);

        vertices.emplace_back(pos1, normal, uv1, tangent2, bitangent2);
        vertices.emplace_back(pos4, normal, uv4, tangent2, bitangent2);
        vertices.emplace_back(pos3, normal, uv3, tangent2, bitangent2);

        return vertices;
    }


    inline std::vector<engine::Vertex> generateBillboardVertices(float uvScale)
    {
        const glm::vec3 normal(0.0f, 0.0f, 1.0f);      // facing forward
        const glm::vec3 tangent(1.0f, 0.0f, 0.0f);     // horizontal right
        const glm::vec3 bitangent(0.0f, 1.0f, 0.0f);   // vertical up

        // Quad corners, counter-clockwise order
        glm::vec3 positions[] = {
            { -0.5f, -0.5f, 0.0f },  // bottom-left
            {  0.5f, -0.5f, 0.0f },  // bottom-right
            {  0.5f,  0.5f, 0.0f },  // top-right
            { -0.5f,  0.5f, 0.0f }   // top-left
        };

        // UVs (with uv scaling)
        glm::vec2 uvs[] = {
            { 0.0f, 0.0f },
            { 1.0f * uvScale, 0.0f },
            { 1.0f * uvScale, 1.0f * uvScale },
            { 0.0f, 1.0f * uvScale }
        };

        // Define the two triangles (CCW winding)
        std::vector<engine::Vertex> vertices;
        vertices.reserve(6);

        // Triangle 1
        vertices.emplace_back( positions[0], normal, uvs[0], tangent, bitangent );
        vertices.emplace_back( positions[1], normal, uvs[1], tangent, bitangent );
        vertices.emplace_back( positions[2], normal, uvs[2], tangent, bitangent );

        // Triangle 2
        vertices.emplace_back( positions[0], normal, uvs[0], tangent, bitangent );
        vertices.emplace_back( positions[2], normal, uvs[2], tangent, bitangent );
        vertices.emplace_back( positions[3], normal, uvs[3], tangent, bitangent );

        return vertices;
    }


    inline std::vector<engine::Vertex> generateCubeVertices()
    {
        std::vector<engine::Vertex> vertices{};
        vertices.reserve(36);

        // Back face
        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec2{ 0.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec2{ 1.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ 1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec2{ 1.0f, 0.0f });

        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec2{ 1.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec2{ 0.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 0.0f, -1.0f }, glm::vec2{ 0.0f, 1.0f });

        // Front face
        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ 1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 1.0f });

        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 1.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec2{ 0.0f, 0.0f });

        // Left face
        vertices.emplace_back(glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f });

        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f });

        // Right face
        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ 1.0f, -1.0f, -1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f, -1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 1.0f });

        vertices.emplace_back(glm::vec3{ 1.0f, -1.0f, -1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ 1.0f, -1.0f,  1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f });

        // Bottom face
        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ 1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, glm::vec2{ 1.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ 1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f });

        vertices.emplace_back(glm::vec3{ 1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{ 0.0f, -1.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f });

        // Top face
        vertices.emplace_back(glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f , 1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 1.0f, 1.0f });

        vertices.emplace_back(glm::vec3{ 1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 1.0f, 0.0f });
        vertices.emplace_back(glm::vec3{ -1.0f,  1.0f, -1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 0.0f, 1.0f });
        vertices.emplace_back(glm::vec3{ -1.0f,  1.0f,  1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f });



        // Compute tangents and bitangents
        for (size_t i = 0; i < vertices.size(); i += 3) {
            glm::vec3 edge1 = vertices[i + 1].position - vertices[i].position;
            glm::vec3 edge2 = vertices[i + 2].position - vertices[i].position;
            glm::vec2 deltaUV1 = vertices[i + 1].texCoords - vertices[i].texCoords;
            glm::vec2 deltaUV2 = vertices[i + 2].texCoords - vertices[i].texCoords;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            glm::vec3 tangent;
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
            tangent = glm::normalize(tangent);

            glm::vec3 bitangent;
            bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
            bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
            bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
            bitangent = glm::normalize(bitangent);

            vertices[i].tangent = tangent;
            vertices[i + 1].tangent = tangent;
            vertices[i + 2].tangent = tangent;

            vertices[i].bitangent = bitangent;
            vertices[i + 1].bitangent = bitangent;
            vertices[i + 2].bitangent = bitangent;
        }

        return vertices;
    }


    inline std::vector<engine::Vertex> generateSphereVertices(float radius = 1.0f, float uvScale = 1.0f)
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

                glm::vec3 position = radius * glm::vec3(xPos, yPos, zPos);
                glm::vec3 normal = glm::normalize(glm::vec3(xPos, yPos, zPos));
                //glm::vec2 texCoord = { xSegment * uvScale, ySegment * uvScale };
                glm::vec2 texCoord = { (1.0f - xSegment) * uvScale, (1.0f - ySegment) * uvScale };


                // Calculate tangent and bitangent
                glm::vec3 tangent = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), normal));
                glm::vec3 bitangent = glm::normalize(glm::cross(normal, tangent));

                vertices.emplace_back(position, normal, texCoord, tangent, bitangent);
            }
        }

        return vertices;
    }

    inline std::vector<Vertex> generateCylinderVertices(unsigned int sectorCount = 36, float height = 2.0f, float radius = 1.0f, float uvScale = 1.0f)
    {
        std::vector<Vertex> vertices;
        const float PI = 3.14159265359f;
        float halfHeight = height / 2.0f;

        // === Side Surface Vertices ===
        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float sectorAngle = 2 * PI * i / sectorCount;
            float x = cosf(sectorAngle);
            float z = sinf(sectorAngle);

            glm::vec3 normal(x, 0.0f, z);

            // FIX: Flip U coordinate to correct horizontal texture orientation
            float u = (1.0f - (float)i / sectorCount) * uvScale;

            // Bottom vertex
            glm::vec2 texCoord(u, 0.0f);
            vertices.emplace_back( glm::vec3(radius * x, -halfHeight, radius * z), normal, texCoord );

            // Top vertex
            texCoord.y = uvScale;
            vertices.emplace_back( glm::vec3(radius * x, halfHeight, radius * z), normal, texCoord );
        }

        // === Top Cap Center Vertex ===
        glm::vec3 topCenter(0.0f, halfHeight, 0.0f);
        glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
        glm::vec2 topTex(0.5f * uvScale, 0.5f * uvScale);
        vertices.emplace_back( topCenter, topNormal, topTex );

        // === Top Cap Ring Vertices ===
        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float angle = 2 * PI * i / sectorCount;
            float x = cosf(angle);
            float z = sinf(angle);
            glm::vec3 pos(radius * x, halfHeight, radius * z);

            glm::vec2 tex(0.5f * (x + 1.0f) * uvScale, 0.5f * (z + 1.0f) * uvScale);
            vertices.emplace_back( pos, topNormal, tex );
        }

        // === Bottom Cap Center Vertex ===
        glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f);
        glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
        glm::vec2 bottomTex(0.5f * uvScale, 0.5f * uvScale);
        vertices.emplace_back( bottomCenter, bottomNormal, bottomTex );

        // === Bottom Cap Ring Vertices ===
        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float angle = 2 * PI * i / sectorCount;
            float x = cosf(angle);
            float z = sinf(angle);
            glm::vec3 pos(radius * x, -halfHeight, radius * z);

            glm::vec2 tex(0.5f * (x + 1.0f) * uvScale, 0.5f * (z + 1.0f) * uvScale);
            vertices.emplace_back( pos, bottomNormal, tex );
        }

        return vertices;
    }

    inline std::vector<Vertex> generateConeVertices(unsigned int sectorCount, float height, float radius, float uvScale)
    {
        std::vector<Vertex> vertices;
        const float PI = 3.14159265359f;
        float halfHeight = height / 2.0f;
        glm::vec3 tip(0.0f, halfHeight, 0.0f);

        // === Side vertices ===
        // Tip vertex (single)
        glm::vec3 tipNormal(0.0f, 1.0f, 0.0f); // We'll compute actual smooth normals below
        glm::vec2 tipUV(0.5f * uvScale, 1.0f * uvScale);
        vertices.emplace_back( tip, tipNormal, tipUV );

        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float angle = 2 * PI * i / sectorCount;
            float x = cosf(angle);
            float z = sinf(angle);

            glm::vec3 pos(radius * x, -halfHeight, radius * z);
            glm::vec3 dir = glm::normalize(glm::vec3(x, radius / height, z)); // Approximate normal
            glm::vec2 uv((1.0f - (float)i / sectorCount) * uvScale, 0.0f); // Flip u so texture isn't mirrored

            vertices.emplace_back( pos, dir, uv );
        }

        // === Base center vertex ===
        glm::vec3 baseCenter(0.0f, -halfHeight, 0.0f);
        glm::vec3 baseNormal(0.0f, -1.0f, 0.0f);
        glm::vec2 baseUV(0.5f * uvScale, 0.5f * uvScale);
        vertices.emplace_back( baseCenter, baseNormal, baseUV );

        // === Base ring vertices ===
        for (unsigned int i = 0; i <= sectorCount; ++i)
        {
            float angle = 2 * PI * i / sectorCount;
            float x = cosf(angle);
            float z = sinf(angle);

            glm::vec3 pos(radius * x, -halfHeight, radius * z);
            glm::vec2 uv(0.5f * (x + 1.0f) * uvScale, 0.5f * (z + 1.0f) * uvScale);
            vertices.emplace_back( pos, baseNormal, uv );
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