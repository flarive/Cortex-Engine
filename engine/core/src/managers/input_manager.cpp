#include "../../include/managers/input_manager.h"

void engine::InputManager::beginFrame()
{
	m_previousKeys = m_currentKeys;

	m_mouseDeltaX = 0.0f;
	m_mouseDeltaY = 0.0f;

	m_scrollX = 0.0f;
	m_scrollY = 0.0f;
}

void engine::InputManager::onKey(int key, int scancode,	int action,	int mods)
{
	if (key < 0 || key >= KEY_COUNT)
		return;

	m_currentKeys[key] = (action != GLFW_RELEASE);
}

void engine::InputManager::onMouseMove(double x, double y)
{
	if (m_firstMouse)
	{
		m_mouseX = static_cast<float>(x);
		m_mouseY = static_cast<float>(y);

		m_previousMouseX = m_mouseX;
		m_previousMouseY = m_mouseY;

		m_firstMouse = false;
		return;
	}

	m_mouseX = static_cast<float>(x);
	m_mouseY = static_cast<float>(y);

	m_mouseDeltaX = m_mouseX - m_previousMouseX;
	m_mouseDeltaY = m_previousMouseY - m_mouseY;

	m_previousMouseX = m_mouseX;
	m_previousMouseY = m_mouseY;
}

void engine::InputManager::onScroll(double xoffset, double yoffset)
{
	m_scrollX += static_cast<float>(xoffset);
	m_scrollY += static_cast<float>(yoffset);
}

bool engine::InputManager::isKeyDown(int key) const
{
	if (key < 0 || key >= KEY_COUNT)
		return false;

	return m_currentKeys[key];
}

bool engine::InputManager::isKeyPressed(int key) const
{
	if (key < 0 || key >= KEY_COUNT)
		return false;

	return m_currentKeys[key] && !m_previousKeys[key];
}

bool engine::InputManager::isKeyReleased(int key) const
{
	if (key < 0 || key >= KEY_COUNT)
		return false;

	return !m_currentKeys[key] && m_previousKeys[key];
}

bool engine::InputManager::isMouseButtonDown(int button) const
{
	if (button < 0 || button >= static_cast<int>(MOUSE_BUTTON_COUNT))
		return false;

	return m_mouseButtons[button];
}

float engine::InputManager::getMouseDeltaX() const
{
	return m_mouseDeltaX;
}

float engine::InputManager::getMouseDeltaY() const
{
	return m_mouseDeltaY;
}

float engine::InputManager::getScrollX() const
{
	return m_scrollX;
}

float engine::InputManager::getScrollY() const
{
	return m_scrollY;
}

