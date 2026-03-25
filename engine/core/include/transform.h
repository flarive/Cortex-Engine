#pragma once

#include "misc/noncopyable.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

namespace engine
{
    struct Transform
    {
    public:
        Transform(glm::vec3 pos = { 0,0,0 }, glm::vec3 scale = { 1,1,1 }, glm::vec3 rot = { 0,0,0 });


        //Transform(const Transform&) = default;
        //Transform& operator=(const Transform&) = default;

        //Transform(Transform&&) = default;
        //Transform& operator=(Transform&&) = default;


        // Build TRS matrix from local values
        glm::mat4 getLocalModelMatrix() const;

        // Local setters
        void setLocalPosition(const glm::vec3& newPosition);
        void setLocalRotation(const glm::vec3& newRotation);
        void setLocalScale(const glm::vec3& newScale);

        // Accessors
        glm::vec3& getLocalPosition();
        glm::vec3& getLocalRotation();
        glm::vec3& getLocalScale();

        // Derived directions (from last world matrix passed in by Entity)
        glm::vec3 getRight(const glm::mat4& world) const;
        glm::vec3 getUp(const glm::mat4& world) const;
        glm::vec3 getForward(const glm::mat4& world) const;
        glm::vec3 getBackward(const glm::mat4& world) const;
        glm::vec3 getGlobalScale(const glm::mat4& world) const;

    private:
        glm::vec3 m_pos{};
        glm::vec3 m_scale{};
        glm::vec3 m_eulerRot{};
    };

    struct AnimTransform final : public Transform
    {
    public:
        AnimTransform() = default;
        AnimTransform(const Transform& from, const Transform& to);
        ~AnimTransform() = default;

        void setFrom(const Transform& from) { m_from = from; }
        void setTo(const Transform& to) { m_to = to; }
    
    private:
        Transform m_from{};
        Transform m_to{};
    };
}
