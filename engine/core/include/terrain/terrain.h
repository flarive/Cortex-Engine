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
    
    class Terrain final
	{
	public:
		Terrain() = default;
		~Terrain() = default;

        void setup();
        void setup(const std::shared_ptr<Terrain>& material);
        void setup(const std::shared_ptr<Terrain>& material, const UvMapping& uv);

        ordered_map<std::string, EditorProperty> getPublicProperties() {
            return {
            };
        }

        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() {
            return {
            };
        }


        std::vector<Vertex> generateVertices();

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform);


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

        float m_uvScale{ 1.0f };

        glm::vec3 m_position{};
        glm::vec3 m_rotation{};
        glm::vec3 m_scale{};
        
        void geometrySetup();
	};
}

