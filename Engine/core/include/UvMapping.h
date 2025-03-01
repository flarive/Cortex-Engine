#pragma once

#include "misc/noncopyable.h"

namespace engine
{
	class UvMapping : private NonCopyable
    {
    public:
        UvMapping() = default;
        UvMapping(float uvScale);
        virtual ~UvMapping() = default;

        float getUvScale() const { return m_uvScale; }

    private:
        float m_uvScale = 1.0f;
    };
}