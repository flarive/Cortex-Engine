#include "../../include/tools/helpers.h"

engine::NormalizedRotation engine::Helpers::normalizeRotation(glm::vec3 rotation)
{
    engine::NormalizedRotation result{};

    if (rotation.x > 0.0f)
    {
        result.angle = rotation.x;
        result.axis.x = 1.0;
    }

    if (rotation.y > 0.0f)
    {
        result.angle = rotation.y;
        result.axis.y = 1.0;
    }

    if (rotation.z > 0.0f)
    {
        result.angle = rotation.z;
        result.axis.z = 1.0;
    }

    return result;
}