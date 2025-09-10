#pragma once

#include "../common_defines.h"

#include "../shader.h"

namespace engine
{
	class Component
	{
    public:
        Component() = default;
        virtual ~Component() = default;

        virtual void init() = 0;
        virtual void update() = 0;
        virtual void draw(Shader& shader, const glm::mat4& transform) = 0;
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

