#include "../include/aabb.h"


engine::AABB::AABB(const glm::vec3& min, const glm::vec3& max)
    : BoundingVolume{}, center{ (max + min) * 0.5f }, extents{ (max - min) * 0.5f }
{
}

engine::AABB::AABB(const glm::vec3& inCenter, float ex, float ey, float ez)
    : BoundingVolume{}, center{ inCenter }, extents{ ex, ey, ez }
{
}


std::array<glm::vec3, 8> engine::AABB::getVertice() const
{
    return {
        glm::vec3{ center.x - extents.x, center.y - extents.y, center.z - extents.z },
        glm::vec3{ center.x + extents.x, center.y - extents.y, center.z - extents.z },
        glm::vec3{ center.x - extents.x, center.y + extents.y, center.z - extents.z },
        glm::vec3{ center.x + extents.x, center.y + extents.y, center.z - extents.z },
        glm::vec3{ center.x - extents.x, center.y - extents.y, center.z + extents.z },
        glm::vec3{ center.x + extents.x, center.y - extents.y, center.z + extents.z },
        glm::vec3{ center.x - extents.x, center.y + extents.y, center.z + extents.z },
        glm::vec3{ center.x + extents.x, center.y + extents.y, center.z + extents.z }
    };
}

bool engine::AABB::isOnOrForwardPlane(const BoundingVolumePlane& plane) const
{
    float r = extents.x * std::abs(plane.normal.x) +
        extents.y * std::abs(plane.normal.y) +
        extents.z * std::abs(plane.normal.z);

    return -r <= plane.getSignedDistanceToPlane(center);
}

bool engine::AABB::isOnFrustum(const Frustum& camFrustum, const glm::mat4& worldTransform) const
{
    // Transform local center
    glm::vec3 globalCenter = glm::vec3(worldTransform * glm::vec4(center, 1.0f));

    // Apply orientation & scale
    glm::vec3 right = glm::vec3(worldTransform[0]) * extents.x;
    glm::vec3 up = glm::vec3(worldTransform[1]) * extents.y;
    glm::vec3 forward = glm::vec3(worldTransform[2]) * extents.z;

    float newEx = std::abs(right.x) + std::abs(up.x) + std::abs(forward.x);
    float newEy = std::abs(right.y) + std::abs(up.y) + std::abs(forward.y);
    float newEz = std::abs(right.z) + std::abs(up.z) + std::abs(forward.z);

    AABB globalAABB(globalCenter, newEx, newEy, newEz);

    return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
        globalAABB.isOnOrForwardPlane(camFrustum.farFace));
}



engine::SquareAABB::SquareAABB(const glm::vec3& inCenter, float inExtent)
    : BoundingVolume{}, center{ inCenter }, extent{ inExtent }
{
}


bool engine::SquareAABB::isOnOrForwardPlane(const BoundingVolumePlane& plane) const
{
    const float r = extent * (std::abs(plane.normal.x) +
        std::abs(plane.normal.y) +
        std::abs(plane.normal.z));
    return -r <= plane.getSignedDistanceToPlane(center);
}

bool  engine::SquareAABB::isOnFrustum(const Frustum& frustum, const glm::mat4& worldTransform) const
{
    glm::vec3 globalCenter = glm::vec3(worldTransform * glm::vec4(center, 1.f));

    glm::vec3 right = glm::vec3(worldTransform[0]) * extent;
    glm::vec3 up = glm::vec3(worldTransform[1]) * extent;
    glm::vec3 forward = glm::vec3(worldTransform[2]) * extent;

    auto checkPlane = [&](const BoundingVolumePlane& plane) -> bool
        {
            float r = std::abs(glm::dot(plane.normal, right)) +
                std::abs(glm::dot(plane.normal, up)) +
                std::abs(glm::dot(plane.normal, forward));

            float d = plane.getSignedDistanceToPlane(globalCenter);
            return (d + r >= 0.f);
        };

    return checkPlane(frustum.leftFace) &&
        checkPlane(frustum.rightFace) &&
        checkPlane(frustum.topFace) &&
        checkPlane(frustum.bottomFace) &&
        checkPlane(frustum.nearFace) &&
        checkPlane(frustum.farFace);
}