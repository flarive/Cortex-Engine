#include "../../include/managers/input_manager.h"

void engine::InputManager::beginFrame()
{
	m_previousKeys = m_currentKeys;

	m_mouseDeltaX = 0.0f;
	m_mouseDeltaY = 0.0f;

	m_scrollOffset = 0.0f;
}

void engine::InputManager::update()
{
	m_previousKeys = m_currentKeys;

	m_mouseDeltaX = m_mouseX - m_lastMouseX;
	m_mouseDeltaY = m_mouseY - m_lastMouseY;

	m_lastMouseX = m_mouseX;
	m_lastMouseY = m_mouseY;

	m_scrollOffset = 0.f;
}

void engine::InputManager::onKey(int key, int scancode,	int action,	int mods)
{
	if (key < 0 || key >= KEY_COUNT)
		return;

	m_currentKeys[key] = (action != GLFW_RELEASE);
}

void engine::InputManager::onMouseMove(double x, double y)
{

}

void engine::InputManager::onScroll(double xoffset, double yoffset)
{

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
	// temp
	return false;
}

float engine::InputManager::getMouseDeltaX() const
{
	// temp
	return 0.0f;
}

float engine::InputManager::getMouseDeltaY() const
{
	// temp
	return 0.0f;
}
