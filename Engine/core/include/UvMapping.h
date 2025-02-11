#pragma once

namespace engine
{
    class UvMapping
    {
    public:
        UvMapping() = default;
        UvMapping(float uvScale);
        virtual ~UvMapping() = default;

        float getUvScale() const { return m_uvScale; }

    private:
        float m_uvScale = 0.5f;
    };
}