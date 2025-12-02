#pragma once

#include "misc/noncopyable.h"
#include "common_defines.h"


#define MAX_BONE_INFLUENCE 4

namespace engine
{
    struct VertexAL final {
        glm::vec3 position{};
        glm::vec3 normal{};
        glm::vec2 texcoord{};
    };


    
    struct Vertex final : private NonCopyableButMovable
    {
        Vertex(const glm::vec3& _pos)
            : Vertex(_pos, glm::vec3(), glm::vec2(), glm::vec3(), glm::vec3())
        {
        }

        Vertex(const glm::vec3& _pos, const glm::vec3& _normal, const glm::vec2& _uv)
            : Vertex(_pos, _normal, _uv, glm::vec3(), glm::vec3())
        {
        }

        Vertex(const glm::vec3& _pos, const glm::vec3& _normal, const glm::vec2& _uv, const glm::vec3& _tangent, const glm::vec3& _bitangent)
            : position(_pos), normal(_normal), texCoords(_uv), tangent(_tangent), bitangent(_bitangent)
        {
        }

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
        int boneIDs[MAX_BONE_INFLUENCE]{};

        //weights from each bone
        float weights[MAX_BONE_INFLUENCE]{};
    };
}
