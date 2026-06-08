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
    const int TERRAIN_TESSELLATION_PATCH_COUNT = 4;
    
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
		Terrain(float heightFactor = 1.0f, unsigned int resolution = 20);
		~Terrain();

        void setup();
        void setup(const std::shared_ptr<Material>& material);
        void setup(const std::shared_ptr<Material>& material, const UvMapping& uv);

        ordered_map<std::string, EditorProperty> getPublicProperties() {
            return {
                {"resolution", EditorProperty { "Resolution", getResolution(), editable, 1.0f, 25.0f, 1.0f, "%.0f" }},
                {"heightfactor", EditorProperty { "Height", getHeightFactor(), editable, 0.0f, 100.0f, 0.1f, "%.1f" }},
                {"offset", EditorProperty { "Offset", getHeightOffset(), editable, 0.0f, 100.0f, 1.0f, "%.0f" }},
                {"uvscale", EditorProperty { "UV scale", getUvScale(), editable, 0.0f, 10.0f, 0.01f, "%.3f"}},
                {"canCastShadows", EditorProperty { "Cast shadows", canCastShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }},
                {"canReceiveShadows", EditorProperty { "Receive shadows", canReceiveShadows(), editable, 0.0f, 10.0f, 0.01f, "%.3f" }}
            };
        }

        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() {
            return {
                { "resolution", [this](EditorPropertyValue value) { getResolution() = *(std::get_if<unsigned int>(&value)); } },
                { "heightfactor", [this](EditorPropertyValue value) { getHeightFactor() = *(std::get_if<float>(&value)); } },
                { "offset", [this](EditorPropertyValue value) { getHeightOffset() = *(std::get_if<glm::vec2>(&value)); } },
                { "uvscale", [this](EditorPropertyValue value) { getUvScale() = *(std::get_if<float>(&value)); } },
                { "canCastShadows", [this](EditorPropertyValue value) { canCastShadows() = *(std::get_if<bool>(&value)); } },
                { "canReceiveShadows", [this](EditorPropertyValue value) { canReceiveShadows() = *(std::get_if<bool>(&value)); } }
            };
        }

        void init();

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

        float& getUvScale() { return m_uvScale; }
        void setUvScale(float uvScale) { m_uvScale = uvScale; }

        bool& canCastShadows() { return m_canCastShadows; }
        bool& canReceiveShadows() { return m_canReceiveShadows; }

        void setCanCastShadows(bool canCast) { m_canCastShadows = canCast; }
        void setCanReceiveShadows(bool canReceive) { m_canReceiveShadows = canReceive; }

        unsigned int& getResolution() { return m_resolution; }
        void setResolution(unsigned int resolution) { m_resolution = resolution; }

        float& getHeightFactor() { return m_heightFactor; }
        void setHeightFactor(float factor) { m_heightFactor = factor; }

        glm::vec2& getHeightOffset() { return m_heightOffset; }
        void setHeightOffset(glm::vec2 offset) { m_heightOffset = offset; }

        void reSetup() { init(); };

        void clean();

    private:

		bool m_isEnabled{ true };
        
        unsigned int m_resolution{};

        bool m_canCastShadows{ true };
		bool m_canReceiveShadows{ true };

        std::shared_ptr<Material> m_material{};
        float m_uvScale{ 1.0f };

        std::vector<engine::Vertex> m_vertices{};

        glm::vec3 m_position{};
        glm::vec3 m_rotation{};
        glm::vec3 m_scale{};
        
        int m_textureWidth{};
        int m_textureHeight{};
		float m_heightFactor{ 1.0f };
        glm::vec2 m_heightOffset{};

        unsigned int m_terrainVAO{};
        unsigned int m_terrainVBO{};

        void geometrySetup();
	};
}