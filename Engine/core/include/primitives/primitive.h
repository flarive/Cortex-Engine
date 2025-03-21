#pragma once

#include "../misc/noncopyable.h"
#include "../common_defines.h"
#include "../shader.h"
#include "../uvmapping.h"
#include "../materials/material.h"

namespace engine
{
    /// <summary>
    /// Abstract class for primitives
    /// </summary>
    class Primitive : private NonCopyable
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

        virtual void draw(Shader& shader, const glm::vec3& position, const glm::vec3& size, float rotationAngle = 0.0f, const glm::vec3& rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f)) = 0;

        // optional: de-allocate all resources once they've outlived their purpose
        virtual void clean();

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

    /// <summary>
    /// Set up vertex data for a 2d quad (counter-countwise)
    /// </summary>
    //inline float quadVertices[] = {
    //    // positions            // normals         // texcoords
    //     1.0f, -0.01f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    //    -1.0f, -0.01f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    //    -1.0f, -0.01f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,

    //     1.0f, -0.01f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    //     1.0f, -0.01f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    //    -1.0f, -0.01f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
    //};


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