#pragma once

#include "../common_defines.h"

#include "../shader.h"
#include "../transform.h"
#include "../aabb.h"

namespace engine
{
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

        

        virtual unsigned int getTypeID() const final
        {
            return getStaticTypeID();
        }

        static unsigned int getStaticTypeID()
        {
            static unsigned int typeID = []()
                {
                return 99;
                }();
            return typeID;
        }
    };
}

