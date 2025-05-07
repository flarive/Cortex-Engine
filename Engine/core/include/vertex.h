#pragma once

//#include "misc/noncopyable.h"
#include "common_defines.h"


#define MAX_BONE_INFLUENCE 4

namespace engine
{
    struct Vertex// : private NonCopyable
    {
        // position
        glm::vec3 position{};
        // normal
        glm::vec3 normal{};
        // texCoords
        glm::vec2 texCoords{};
        // tangent
        glm::vec3 tangent{};
        // bitangent
        glm::vec3 bitangent{};
        //bone indexes which will influence this vertex
        int m_BoneIDs[MAX_BONE_INFLUENCE]{};
        //weights from each bone
        float m_Weights[MAX_BONE_INFLUENCE]{};
    };
}
