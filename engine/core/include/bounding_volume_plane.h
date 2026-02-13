#pragma once

#include "common_defines.h"

namespace engine
{
	struct BoundingVolumePlane
	{
		glm::vec3 normal = { 0.f, 1.f, 0.f }; // unit vector
		float distance{};        // Distance with origin

		BoundingVolumePlane() = default;
		BoundingVolumePlane(const glm::vec3& p1, const glm::vec3& norm);

		float getSignedDistanceToPlane(const glm::vec3& point) const;
	};
}
