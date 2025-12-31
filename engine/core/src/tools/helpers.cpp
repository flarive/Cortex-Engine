#include "../../include/tools/helpers.h"

void engine::Helpers::printMat4(const glm::mat4& mat)
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << mat[j][i] << " ";
        }
        std::cout << std::endl;
    }
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