#include "../include/transform.h"


engine::Transform::Transform(glm::vec3 _position, glm::vec3 _scale, glm::vec3 _rotation) : m_pos(_position), m_scale(_scale), m_eulerRot(_rotation)
{
}

glm::mat4 engine::Transform::getLocalModelMatrix()
{
	const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Y * X * Z
	const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

	// translation * rotation * scale (also know as TRS matrix)
	return glm::translate(glm::mat4(1.0f), m_pos) * rotationMatrix * glm::scale(glm::mat4(1.0f), m_scale);
}

void engine::Transform::computeModelMatrix()
{
	m_modelMatrix = getLocalModelMatrix();
	m_isDirty = false;
}

void engine::Transform::computeModelMatrix(const glm::mat4& parentGlobalModelMatrix)
{
	m_modelMatrix = parentGlobalModelMatrix * getLocalModelMatrix();
	m_isDirty = false;
}

void engine::Transform::setLocalPosition(const glm::vec3& newPosition)
{
	m_pos = newPosition;
	m_isDirty = true;
}

void engine::Transform::setLocalRotation(const glm::vec3& newRotation)
{
	m_eulerRot = newRotation;
	m_isDirty = true;
}

void engine::Transform::setLocalScale(const glm::vec3& newScale)
{
	m_scale = newScale;
	m_isDirty = true;
}

const glm::vec3& engine::Transform::getGlobalPosition() const
{
	return m_modelMatrix[3];
}

const glm::vec3& engine::Transform::getLocalPosition() const
{
	return m_pos;
}

const glm::vec3& engine::Transform::getLocalRotation() const
{
	return m_eulerRot;
}

const glm::vec3& engine::Transform::getLocalScale() const
{
	return m_scale;
}

const glm::mat4& engine::Transform::getModelMatrix() const
{
	return m_modelMatrix;
}

glm::vec3 engine::Transform::getRight() const
{
	return m_modelMatrix[0];
}


glm::vec3 engine::Transform::getUp() const
{
	return m_modelMatrix[1];
}

glm::vec3 engine::Transform::getBackward() const
{
	return m_modelMatrix[2];
}

glm::vec3 engine::Transform::getForward() const
{
	return -m_modelMatrix[2];
}

glm::vec3 engine::Transform::getGlobalScale() const
{
	return { glm::length(getRight()), glm::length(getUp()), glm::length(getBackward()) };
}

bool engine::Transform::isDirty() const
{
	return m_isDirty;
}

void engine::Transform::clearDirty()
{
	m_isDirty = false;
}
