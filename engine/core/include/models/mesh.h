#pragma once

#include <vector>

#include "../misc/noncopyable.h"

#include "../vertex.h"
#include "../materials/material.h"
#include "../debug/debug_draw_line.h"
#include "../shader.h"

namespace engine
{
	class Mesh final : private NonCopyableButMovable
    {
    public:
        // constructor
        Mesh(const std::string& _name, std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::shared_ptr<Material> _material);
		~Mesh();

        // override a little NonCopyableButMovable
        Mesh(Mesh&&) = default;

        // render the mesh
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix);

        std::shared_ptr<Material> getMaterial() { return m_material; }

		const std::string& getName() const { return m_name; }

        const std::vector<Vertex>& getVertices() const { return m_vertices; }
        const std::vector<unsigned int>& getIndices() const { return m_indices; }


        /*const std::vector<glm::mat4>& getBindPoseMatrices() const { return m_bindPoseMatrices; }
        const glm::mat4& getBindPoseMatrice(int index) const { return m_bindPoseMatrices[index]; }
        void setBindPoseMatrices(const std::vector<glm::mat4>& matrice) { m_bindPoseMatrices = matrice; }*/

        

        //const bool hasBones() const { return m_hasBones; }
        //void setHasBones(bool hasBones) { m_hasBones = hasBones; }

        const bool hasAnimations() const { return m_hasAnimations; }
        void setHasAnimations(bool hasAnimations) { m_hasAnimations = hasAnimations; }

        const bool hasNormals() const { return m_hasNormals; }
        void setHasNormals(bool hasNormals) { m_hasNormals = hasNormals; }

        const bool hasTangents() const { return m_hasTangents; }
        void setHasTangents(bool hasTangents) { m_hasTangents = hasTangents; }

        const bool hasTexCoords() const { return m_hasTexCoords; }
        void setHasTexCoords(bool hasTexCoords) { m_hasTexCoords = hasTexCoords; }

        void setNodeTransform(const glm::mat4& transform) { m_nodeTransform = transform; }
        const glm::mat4& getNodeTransform() const { return m_nodeTransform; };

        void setIsSkinned(bool hasSkin) { m_isSkinned = hasSkin; }
        const bool isSkinned() const { return m_isSkinned; }

        void clean();
       
    private:

        std::string m_name{};

        // mesh Data
        std::vector<Vertex> m_vertices{};
        std::vector<unsigned int> m_indices{};

        // render data 
        unsigned int m_VBO{}, m_EBO{}, m_VAO{};

        unsigned int m_indexCount{};

        std::shared_ptr<Material> m_material{};


        glm::mat4 m_nodeTransform{ 1.0f };

        DebugDraw m_debugDrawLine{};



        // Bind-pose fallback
        std::vector<glm::mat4> m_bindPoseMatrices{};
        //bool m_hasBones{ false }; // model level property, not mesh level
        bool m_isSkinned{ false }; // mesh level property
        bool m_hasAnimations{ false };

        bool m_hasNormals{ false };
        bool m_hasTangents{ false };
        bool m_hasTexCoords{ false };

        // initializes all the buffer objects/arrays
        void setupMesh();
        void handleOpacity(Shader& shader);

        void drawDebugNormals(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix);
    };
}
