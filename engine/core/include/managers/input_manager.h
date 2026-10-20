#pragma once

#include "../common_defines.h"

#include <array>

namespace engine
{
    class InputManager final
    {
    public:

        void beginFrame();

        void onKey(int key, int scancode, int action, int mods);
        void onMouseMove(double x, double y);
        void onMouseButton(int button, int action, int mods);
        void onScroll(double xoffset, double yoffset);

        
        void updateGamepad();
        
        bool isKeyDown(int key) const;
        bool isKeyPressed(int key) const;
        bool isKeyReleased(int key) const;

        bool isMouseButtonDown(int button) const;
        bool isMouseButtonPressed(int button) const;
        bool isMouseButtonReleased(int button) const;


        float getMouseDeltaX() const;
        float getMouseDeltaY() const;

        float getScrollX() const;
        float getScrollY() const;

        bool isGamepadPresent() const;
        bool isGamepadButtonDown(int button) const;
        bool isGamepadButtonPressed(int button) const;
        bool isGamepadButtonReleased(int button) const;

        float getGamepadAxis(int axis) const;

    private:
        static constexpr size_t KEY_COUNT = GLFW_KEY_LAST + 1;
        std::array<bool, KEY_COUNT> m_currentKeys{};
        std::array<bool, KEY_COUNT> m_previousKeys{};

        static constexpr size_t MOUSE_BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST + 1;
        std::array<bool, MOUSE_BUTTON_COUNT> m_mouseButtons{};
        std::array<bool, MOUSE_BUTTON_COUNT> m_previousMouseButtons{};
        

        float m_mouseX{};
        float m_mouseY{};

        float m_previousMouseX{};
        float m_previousMouseY{};

        float m_mouseDeltaX{};
        float m_mouseDeltaY{};

        bool m_firstMouse{ true };

   
        float m_scrollX{};
        float m_scrollY{};

        GLFWgamepadstate m_currentGamepad{};
        GLFWgamepadstate m_previousGamepad{};
        bool m_gamepadConnected = false;
    };
}