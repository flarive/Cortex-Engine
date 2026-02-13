#include "../../include/tools/helpers.h"

#include "../../include/managers/log_manager.h"

#include <sstream>
#include <fmt/core.h>

void engine::Helpers::printMat4(const glm::mat4& mat)
{
    std::ostringstream matrixStream;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            matrixStream << mat[j][i] << " ";
        }
        matrixStream << std::endl;
    }

    // Log the full matrix as a string
    logger.info("Matrice {}", matrixStream.str());
}

//engine::NormalizedRotation engine::Helpers::normalizeRotation(glm::vec3 rotation)
//{
//    engine::NormalizedRotation result{};
//
//    if (rotation.x != 0.0f)
//    {
//        result.angle = rotation.x;
//        result.axis.x = 1.0;
//    }
//
//    if (rotation.y != 0.0f)
//    {
//        result.angle = rotation.y;
//        result.axis.y = 1.0;
//    }
//
//    if (rotation.z != 0.0f)
//    {
//        result.angle = rotation.z;
//        result.axis.z = 1.0;
//    }
//
//    return result;
//}