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

namespace Engine
{
	class Terrain final
	{
	public:
		Terrain() = default;
		~Terrain() = default;

        void setup() override;
        void setup(const std::shared_ptr<Terrain>& material) override;
        void setup(const std::shared_ptr<Terrain>& material, const UvMapping& uv) override;

        ordered_map<std::string, EditorProperty> getPublicProperties() override {
            return {
            };
        }

        std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() override {
            return {
            };
        }


        std::vector<Vertex> generateVertices() override;

        // draws the model, and thus all its meshes
        void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) override;


        PrimitiveType getTypeID() const override
        {
            return PrimitiveType::billboard;
        }

        void clean() override;

    private:
        void geometrySetup();
	};
}

