#pragma once

#include "../common_defines.h"

#include <array>

namespace engine
{
    /*enum class Key
    {
        Unknown = 0,

        W,
        A,
        S,
        D,

        Space,
        Escape,

        LeftShift,
        LeftCtrl,

        F1,
        F2,
        F3,

        Count
    };

    enum class MouseButton
    {
        Left = 0,
        Right,
        Middle,

        Count
    };*/

    class InputManager final
    {
    public:

        void update();

        void onKey(int key, int scancode, int action, int mods);
        void onMouseMove(double x, double y);
        void onScroll(double xoffset, double yoffset);

        
        bool isKeyDown(int key) const;
        bool isKeyPressed(int key) const;
        bool isKeyReleased(int key) const;

        bool isMouseButtonDown(int button) const;

        float getMouseDeltaX() const;
        float getMouseDeltaY() const;

    private:
        static constexpr size_t KEY_COUNT = GLFW_KEY_LAST + 1;

        std::array<bool, KEY_COUNT> m_currentKeys{};
        std::array<bool, KEY_COUNT> m_previousKeys{};

        

        double m_mouseX{};
        double m_mouseY{};

        double m_mouseDeltaX{};
        double m_mouseDeltaY{};

        double m_lastMouseX{};
        double m_lastMouseY{};

        float m_scrollOffset{};
    };
}