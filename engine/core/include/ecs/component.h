#pragma once

#include "../common_defines.h"

#include "../shader.h"
#include "../transform.h"
#include "../aabb.h"

#include <vector>
#include <variant>

namespace engine
{
    enum class ComponentType { undefined = 0, transform = 1, camera = 2, primitive = 3, model = 4, light = 5 };

	struct KeyValuePair
    {
        std::string key{};
        std::variant<int, std::string, float> value{};
	};;

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

        virtual void draw(glm::mat4 projection, glm::mat4 view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform) = 0;

        virtual engine::AABB* getBoundingVolume();

        virtual std::string getName() = 0;


        virtual std::vector<KeyValuePair> getPublicProperties() = 0;



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

