#pragma once

#include "bounding_volume_plane.h"

namespace engine
{
	/// <summary>
	/// Frustrum is the region of space in the modeled world that may appear on the screen
	/// https://en.wikipedia.org/wiki/Viewing_frustum
	/// </summary>
	struct Frustum
	{
		BoundingVolumePlane topFace;
		BoundingVolumePlane bottomFace;

		BoundingVolumePlane rightFace;
		BoundingVolumePlane leftFace;

		BoundingVolumePlane farFace;
		BoundingVolumePlane nearFace;
	};
}