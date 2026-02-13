#pragma once

#include "common_defines.h"
#include "frustrum.h"
#include "bounding_volume_plane.h"

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

    //struct SphereVolume : public BoundingVolume
    //{
    //    glm::vec3 center{ 0.f, 0.f, 0.f }; // local center
    //    float radius{ 0.f };                // local radius

    //    SphereVolume(const glm::vec3& inCenter, float inRadius);

    //    // Test in local/object space
    //    bool isOnOrForwardPlane(const BoundingVolumePlane& plane) const final;

    //    bool isOnFrustum(const Frustum& frustum, const glm::mat4& worldTransform) const;
    //};
}