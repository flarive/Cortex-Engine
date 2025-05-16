#pragma once

#include "common_defines.h"

#include "frustrum.h"
#include "transform.h"
#include "bounding_volume_plane.h"

namespace engine
{
	struct BoundingVolume
	{
		virtual bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const = 0;

		virtual bool isOnOrForwardPlane(const BoundingVolumePlane& plane) const = 0;

		bool isOnFrustum(const Frustum& camFrustum) const
		{
			return (isOnOrForwardPlane(camFrustum.leftFace) &&
				isOnOrForwardPlane(camFrustum.rightFace) &&
				isOnOrForwardPlane(camFrustum.topFace) &&
				isOnOrForwardPlane(camFrustum.bottomFace) &&
				isOnOrForwardPlane(camFrustum.nearFace) &&
				isOnOrForwardPlane(camFrustum.farFace));
		};
	};



	struct SphereVolume : public BoundingVolume
	{
		glm::vec3 center{ 0.f, 0.f, 0.f };
		float radius{ 0.f };

		SphereVolume(const glm::vec3& inCenter, float inRadius)
			: BoundingVolume{}, center{ inCenter }, radius{ inRadius }
		{
		}

		bool isOnOrForwardPlane(const BoundingVolumePlane& plane) const final
		{
			return plane.getSignedDistanceToPlane(center) > -radius;
		}

		bool isOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
		{
			//Get global scale thanks to our transform
			const glm::vec3 globalScale = transform.getGlobalScale();

			//Get our global center with process it with the global model matrix of our transform
			const glm::vec3 globalCenter{ transform.getModelMatrix() * glm::vec4(center, 1.f) };

			//To wrap correctly our shape, we need the maximum scale scalar.
			const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

			//Max scale is assuming for the diameter. So, we need the half to apply it to our radius
			SphereVolume globalSphere(globalCenter, radius * (maxScale * 0.5f));

			//Check Firstly the result that have the most chance to failure to avoid to call all functions.
			return (globalSphere.isOnOrForwardPlane(camFrustum.leftFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.rightFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.farFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.nearFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.topFace) &&
				globalSphere.isOnOrForwardPlane(camFrustum.bottomFace));
		};
	};
}