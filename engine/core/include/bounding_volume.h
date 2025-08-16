#pragma once

#include "common_defines.h"
#include "frustrum.h"
#include "bounding_volume_plane.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

namespace engine
{
    struct BoundingVolume
    {
        virtual ~BoundingVolume() = default;

        // World-aware test
        virtual bool isOnFrustum(const Frustum& camFrustum, const glm::mat4& worldTransform) const = 0;

        // Raw check against a plane (in local/object space)
        virtual bool isOnOrForwardPlane(const BoundingVolumePlane& plane) const = 0;
    };

    struct SphereVolume : public BoundingVolume
    {
        glm::vec3 center{ 0.f, 0.f, 0.f }; // local center
        float radius{ 0.f };                // local radius

        SphereVolume(const glm::vec3& inCenter, float inRadius)
            : center(inCenter), radius(inRadius) {
        }

        // Test in local/object space
        bool isOnOrForwardPlane(const BoundingVolumePlane& plane) const final
        {
            return plane.getSignedDistanceToPlane(center) > -radius;
        }

        bool isOnFrustum(const Frustum& frustum, const glm::mat4& worldTransform) const
        {
            // Transform sphere center into world space
            glm::vec3 globalCenter = glm::vec3(worldTransform * glm::vec4(center, 1.0f));

            // Extract global scale from worldTransform to scale radius
            glm::vec3 scale;
            scale.x = glm::length(glm::vec3(worldTransform[0]));
            scale.y = glm::length(glm::vec3(worldTransform[1]));
            scale.z = glm::length(glm::vec3(worldTransform[2]));

            float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
            float globalRadius = radius * maxScale;

            auto checkPlane = [&](const BoundingVolumePlane& plane) -> bool
                {
                    // Distance from sphere center to plane
                    float d = plane.getSignedDistanceToPlane(globalCenter);
                    return (d + globalRadius >= 0.f);
                };

            // Check against all frustum planes
            return checkPlane(frustum.leftFace) &&
                checkPlane(frustum.rightFace) &&
                checkPlane(frustum.topFace) &&
                checkPlane(frustum.bottomFace) &&
                checkPlane(frustum.nearFace) &&
                checkPlane(frustum.farFace);
        }
    };
}