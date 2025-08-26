#include "../include/bounding_volume_plane.h"

engine::BoundingVolumePlane::BoundingVolumePlane(const glm::vec3& p1, const glm::vec3& norm)
	: normal(glm::normalize(norm)),	distance(glm::dot(normal, p1))
{
}

float engine::BoundingVolumePlane::getSignedDistanceToPlane(const glm::vec3& point) const
{
	return glm::dot(normal, point) - distance;
}