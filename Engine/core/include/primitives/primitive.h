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
    enum class PrimitiveType { undefined = 0, cube = 1, sphere = 2, plane = 3, cylinder = 4, cone = 5, billboard = 6 };


    const std::unordered_map<PrimitiveType, std::string> PrimitiveTypeNames = {
        {PrimitiveType::undefined, "undefined"},
        {PrimitiveType::cube, "cube"},
        {PrimitiveType::sphere, "sphere"},
        {PrimitiveType::plane, "plane"},
        {PrimitiveType::cylinder, "cylinder"},
        {PrimitiveType::cone, "cone"},
        {PrimitiveType::billboard, "billboard"}
    };

    inline std::string to_string(PrimitiveType type) {
        auto it = PrimitiveTypeNames.find(type);
        return it != PrimitiveTypeNames.end() ? it->second : "unknown";
    }

   
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



        bool m_canCastShadows{ true };
        bool m_canReceiveShadows{ true };

    public:
        
        //glm::vec3 position{};
        //glm::vec3 rotation{};
        //glm::vec3 scale{};

        bool highlight{};
        
        Primitive(const glm::vec3& _position = glm::vec3());
        virtual ~Primitive() = default;

        virtual void setup() = 0;
        virtual void setup(const std::shared_ptr<Material>& material) = 0;
        virtual void setup(const std::shared_ptr<Material>& material, const UvMapping& uv) = 0;

        virtual void draw(Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::mat4& transformMatrix, Transform& localTransform) = 0;

        // optional: de-allocate all resources once they've outlived their purpose
        virtual void clean() = 0;


		virtual ordered_map<std::string, EditorProperty> getPublicProperties() = 0;
        virtual std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() = 0;

        virtual std::vector<Vertex> generateVertices() = 0;

        std::shared_ptr<Material> getMaterial() { return m_material; }

		float& getUvScale() { return m_uvScale; }
        void setUvScale(float uvScale) { m_uvScale = uvScale; }

        bool& canCastShadows() { return m_canCastShadows; }
        bool& canReceiveShadows() { return m_canReceiveShadows; }

        void setCanCastShadows(bool canCast) { m_canCastShadows = canCast; }
        void setCanReceiveShadows(bool canReceive) { m_canReceiveShadows = canReceive; }


        static std::vector<Vertex> generatePlaneVertices(float uvScale = 1.0f, bool flipNormal = false);
        static std::vector<Vertex> generateBillboardVertices(float uvScale = 1.0f);
        static std::vector<Vertex> generateCubeVertices(float uvScale = 1.0f);
        static std::vector<Vertex> generateCuboidVertices(float width = 1.0f, float height = 1.0f, float depth = 1.0f, float uvScale = 1.0f);
        static std::vector<Vertex> generateSphereVertices(float radius = 1.0f, float uvScale = 1.0f);
        static std::vector<Vertex> generateCylinderVertices(unsigned int sectorCount = 36, float height = 2.0f, float radius = 1.0f, float uvScale = 1.0f);
        static std::vector<Vertex> generateConeVertices(unsigned int sectorCount, float height, float radius, float uvScale);


        void reSetup() { setup(); };

        virtual PrimitiveType getTypeID() const
        {
            return PrimitiveType::undefined;
        }

		inline glm::vec3& getPosition() { return m_position; }
        inline glm::vec3& getRotation() { return m_rotation; }
        inline glm::vec3& getScale() { return m_scale; }

        inline void setPosition(const glm::vec3& position) { m_position = position; }
        inline void setRotation(const glm::vec3& rotation) { m_rotation = rotation; }
        inline void setScale(const glm::vec3& scale) { m_scale = scale; }

        inline void setTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
            m_position = position;
            m_rotation = rotation;
            m_scale = scale;
		}

        bool isEnabled() const { return m_isEnabled; }
        void setEnabled(bool enabled) { m_isEnabled = enabled; }

    protected:
        bool m_isEnabled{ true };


    private:
        glm::vec3 m_position{};
        glm::vec3 m_rotation{};
        glm::vec3 m_scale{};

        
        
        virtual void geometrySetup() = 0;
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