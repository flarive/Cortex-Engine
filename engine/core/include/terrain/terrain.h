#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <variant>

#include "../common_defines.h"
#include "../vertex.h"
#include "../shader.h"
#include "../uvmapping.h"
#include "../materials/material.h"
#include "../misc/noncopyable.h"
#include "../misc/ordered_map.h"


#include "../transform.h"

#include "../../include/debug/opengl_debug.h"
#include "../../include/debug/debug_draw_line.h"

namespace engine
{
    enum class TerrainType { undefined = 0, terrain = 1 };


    const std::unordered_map<TerrainType, std::string> terrainTypeNames = {
        {TerrainType::undefined, "undefined"},
        {TerrainType::terrain, "Terrain"}
    };

    inline std::string to_string(TerrainType type) {
        auto it = terrainTypeNames.find(type);
        return it != terrainTypeNames.end() ? it->second : "unknown";
    }
    
    class Terrain final
	{
	public:
		Terrain(unsigned int width = 512, unsigned int height = 512, unsigned int patchCount = 4);
		~Terrain() = default;

        void setup();
        void setup(const std::shared_ptr<Material>& material);
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv);

        ordered_map<std::string, EditorProperty> getPublicProperties() {
            return {
            };
        }

        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() {
            return {
            };
        }

        

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform);

        std::vector<engine::Vertex> generateVertices();


        TerrainType getTypeID() const
        {
            return TerrainType::terrain;
        }

        bool isEnabled() const { return m_isEnabled; }
        void setEnabled(bool enabled) { m_isEnabled = enabled; }

        glm::vec3& getPosition() { return m_position; }
        glm::vec3& getRotation() { return m_rotation; }
        glm::vec3& getScale() { return m_scale; }

        void setPosition(const glm::vec3& position) { m_position = position; }
        void setRotation(const glm::vec3& rotation) { m_rotation = rotation; }
        void setScale(const glm::vec3& scale) { m_scale = scale; }

        void setTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
            m_position = position;
            m_rotation = rotation;
            m_scale = scale;
        }

        void clean();

    private:

		bool m_isEnabled{ true };

        int m_width{};
        int m_height{};
        int m_patchCount{};

        std::shared_ptr<Material> m_material{};
        float m_uvScale{ 1.0f };

        glm::vec3 m_position{};
        glm::vec3 m_rotation{};
        glm::vec3 m_scale{};
        
        Shader m_tessHeightMapShader{};

        unsigned int m_textureId{};
        int m_textureWidth{};
        int m_textureHeight{};

        unsigned int m_terrainVAO{};
        unsigned int m_terrainVBO{};

        unsigned int m_rez{ 20 };

        void loadShaders();
        void geometrySetup();
	};
}