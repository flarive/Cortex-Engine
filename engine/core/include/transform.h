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


        // Build TRS matrix from local values
        glm::mat4 getLocalModelMatrix() const;

        // Setters
        void setLocalPosition(const glm::vec3& p) { m_pos = p; }
        void setLocalRotation(const glm::vec3& r) { m_eulerRot = r; }
        void setLocalScale(const glm::vec3& s) { m_scale = s; }

        // Getters
        glm::vec3& getLocalPosition() { return m_pos; }
        glm::vec3& getLocalRotation() { return m_eulerRot; }
        glm::vec3& getLocalScale() { return m_scale; }

        const glm::vec3& getLocalPosition() const { return m_pos; }
        const glm::vec3& getLocalRotation() const { return m_eulerRot; }
        const glm::vec3& getLocalScale()    const { return m_scale; }


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
}
