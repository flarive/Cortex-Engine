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


        // Fluent add methods
        Transform& addTranslationX(float amount) { m_pos.x += amount; return *this; }
        Transform& addTranslationY(float amount) { m_pos.y += amount; return *this; }
        Transform& addTranslationZ(float amount) { m_pos.z += amount; return *this; }
        Transform& addTranslation(float amount) { m_pos += glm::vec3(amount); return *this; }
        Transform& addTranslation(const glm::vec3& amount) { m_pos += amount; return *this; }

        Transform& addRotationX(float amount) { m_eulerRot.x += amount; return *this; }
        Transform& addRotationY(float amount) { m_eulerRot.y += amount; return *this; }
        Transform& addRotationZ(float amount) { m_eulerRot.z += amount; return *this; }
        Transform& addRotation(float amount) { m_eulerRot += glm::vec3(amount); return *this; }
        Transform& addRotation(const glm::vec3& amount) { m_eulerRot += amount; return *this; }

        Transform& addScaleX(float amount) { m_scale.x += amount; return *this; }
        Transform& addScaleY(float amount) { m_scale.y += amount; return *this; }
        Transform& addScaleZ(float amount) { m_scale.z += amount; return *this; }
        Transform& addScale(float amount) { m_scale += glm::vec3(amount); return *this; }
        Transform& addScale(const glm::vec3& amount) { m_scale += amount; return *this; }

    private:
        glm::vec3 m_pos{};
        glm::vec3 m_scale{};
        glm::vec3 m_eulerRot{};
    };
}
