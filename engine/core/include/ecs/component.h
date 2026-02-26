#pragma once

#include "../common_defines.h"

#include "../shader.h"
#include "../transform.h"
#include "../aabb.h"

#include "../misc/ordered_map.h"

#include <vector>
#include <map>
#include <variant>
#include <unordered_map>
#include <functional>
#include <variant>

namespace engine
{
    enum class ComponentType { undefined = 0, transform = 1, camera = 2, primitive = 3, model = 4, light = 5, animator = 6, particleSystem = 7, terrain = 8 };



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
        virtual void update(float deltaTime, Transform& transform) = 0;

        virtual void draw(const glm::mat4& projection, const glm::mat4& view, Shader& shader, const glm::mat4& worldTransformMatrix, Transform& localTransform, AABB* boundingVolume = nullptr) = 0;

        virtual engine::AABB* getBoundingVolume();

        virtual std::string getName() = 0;

        virtual ordered_map<std::string, EditorProperty> getPublicProperties() = 0;
        virtual std::unordered_map<std::string, std::function<void(EditorPropertyValue)>> getPropertySetters() = 0;

        virtual void setProperty(const std::string& key, EditorPropertyValue value) = 0;

		bool isEnabled() const { return m_isEnabled; }
        virtual void setEnabled(bool enabled) = 0;

    private:
		bool m_isEnabled{ true };
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

