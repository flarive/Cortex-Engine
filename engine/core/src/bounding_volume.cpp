#include "../include/bounding_volume.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
//
//engine::SphereVolume::SphereVolume(const glm::vec3& inCenter, float inRadius)
//    : center(inCenter), radius(inRadius) {
//}
//
//bool engine::SphereVolume::isOnOrForwardPlane(const BoundingVolumePlane& plane) const
//{
//    return plane.getSignedDistanceToPlane(center) > -radius;
//}
//
//bool engine::SphereVolume::isOnFrustum(const Frustum& frustum, const glm::mat4& worldTransform) const
//{
//    // Transform sphere center into world space
//    glm::vec3 globalCenter = glm::vec3(worldTransform * glm::vec4(center, 1.0f));
//
//    // Extract global scale from worldTransform to scale radius
//    glm::vec3 scale;
//    scale.x = glm::length(glm::vec3(worldTransform[0]));
//    scale.y = glm::length(glm::vec3(worldTransform[1]));
//    scale.z = glm::length(glm::vec3(worldTransform[2]));
//
//    float maxScale = std::max(std::max(scale.x, scale.y), scale.z);
//    float globalRadius = radius * maxScale;
//
//    auto checkPlane = [&](const BoundingVolumePlane& plane) -> bool
//        {
//            // Distance from sphere center to plane
//            float d = plane.getSignedDistanceToPlane(globalCenter);
//            return (d + globalRadius >= 0.f);
//        };
//
//    // Check against all frustum planes
//    return checkPlane(frustum.leftFace) &&
//        checkPlane(frustum.rightFace) &&
//        checkPlane(frustum.topFace) &&
//        checkPlane(frustum.bottomFace) &&
//        checkPlane(frustum.nearFace) &&
//        checkPlane(frustum.farFace);
//}