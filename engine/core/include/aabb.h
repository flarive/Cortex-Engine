#pragma once

#include "common_defines.h"
#include "bounding_volume.h"

#include <array>

namespace engine
{
    struct SquareAABB : public BoundingVolume
    {
        glm::vec3 center{ 0.f, 0.f, 0.f };
        float extent{ 0.f };

        SquareAABB(const glm::vec3& inCenter, float inExtent);
        
        bool isOnOrForwardPlane(const BoundingVolumePlane& plane) const final;

        bool isOnFrustum(const Frustum& frustum, const glm::mat4& worldTransform) const final;
    };

    struct AABB : public BoundingVolume
    {
        glm::vec3 center{ 0.f, 0.f, 0.f };
        glm::vec3 extents{ 0.f, 0.f, 0.f };

        AABB(const glm::vec3& min, const glm::vec3& max);

        AABB(const glm::vec3& inCenter, float ex, float ey, float ez);

        std::array<glm::vec3, 8> getVertice() const;

        //see https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
        bool isOnOrForwardPlane(const BoundingVolumePlane& plane) const final;

        bool isOnFrustum(const Frustum& camFrustum, const glm::mat4& worldTransform) const final;
    };
}
