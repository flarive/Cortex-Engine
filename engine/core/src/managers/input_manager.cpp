#include "../../include/managers/input_manager.h"


void engine::InputManager::update()
{
	m_previousKeys = m_currentKeys;

	m_mouseDeltaX = m_mouseX - m_lastMouseX;
	m_mouseDeltaY = m_mouseY - m_lastMouseY;

	m_lastMouseX = m_mouseX;
	m_lastMouseY = m_mouseY;

	m_scrollOffset = 0.f;
}



void engine::InputManager::onKey(int key, int scancode, int action, int mods)
{

}

void engine::InputManager::onMouseMove(double x, double y)
{

}

void engine::InputManager::onScroll(double xoffset, double yoffset)
{

}

bool engine::InputManager::isKeyDown(int key) const
{
	// temp
	return false;
}

bool engine::InputManager::isKeyPressed(int key) const
{
	// temp
	return false;
}

bool engine::InputManager::isKeyReleased(int key) const
{
	// temp
	return false;
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
