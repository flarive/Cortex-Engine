#pragma once

#include "../common_defines.h"

#include "../shader.h"
#include "../transform.h"
#include "../aabb.h"

namespace engine
{
    enum class ComponentType { undefined = 0, transform = 1, camera = 2, primitive = 3, model = 4, light = 5 };

    // faster
    constexpr unsigned int toInt(ComponentType type) {
        return static_cast<unsigned int>(type);
    }

    
    class Component
	{
    public:
        Component() = default;
        virtual ~Component() = default;

        virtual void init(Transform& transform) = 0;
        virtual void update(Transform& transform) = 0;

        virtual void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& transform) = 0;
        virtual void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::vec3& position, const glm::vec3& size, const glm::vec3& rotation) = 0;

        virtual std::unique_ptr<AABB> getBoundingVolume();
	};

    /// <summary>
    /// CRTP
    /// </summary>
    template<typename Derived>
    class ComponentBase : public Component
    {
    public:
        ComponentBase() = default;
        virtual ~ComponentBase() = default;

        

        virtual ComponentType getTypeID() const final
        {
            return getStaticTypeID();
        }

        static ComponentType getStaticTypeID()
        {
            static ComponentType typeID = []()
                {
                return ComponentType::undefined;
                }();
            return typeID;
        }
    };
}

