#include "../../include/cameras/camera.h"

#include "../../include/managers/log_manager.h"

#include <glm/glm.hpp> //glm::mat4
#include <list> //std::list
#include <array> //std::array
#include <memory> //std::unique_ptr

engine::Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _zoom, float _yaw, float _pitch, float _speed, float _sensitivity)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(_speed), mouseSensitivity(_sensitivity), m_zoom(_zoom), yaw(_yaw), pitch(_pitch)
{
    logger.trace("Camera base constructor called");

    position = _position;
    worldUp = _up;
    
    updateCameraVectors();
}

glm::mat4& engine::Camera::getViewMatrix()
{
    if (m_enabled)
        m_viewMatrix = glm::lookAt(position, position + front, up);
    else
		m_viewMatrix = glm::mat4(0.0f);

    return m_viewMatrix;
}

glm::mat4& engine::Camera::getProjectionMatrix(float aspect)
{
    if (m_enabled)
        m_projection = glm::perspective(glm::radians(m_zoom), aspect, m_nearPlane, m_farPlane);
    else
		m_projection = glm::mat4(0.0f);
    
    return m_projection;
}

void engine::Camera::setFromViewMatrix(const glm::mat4& view)
{
    // Inverse view => world transform
    glm::mat4 inv = glm::inverse(view);
    position = glm::vec3(inv[3]); // translation
    front = glm::normalize(-glm::vec3(view[0][2], view[1][2], view[2][2])); // forward
    up = glm::normalize(glm::vec3(view[0][1], view[1][1], view[2][1]));  // up
}

void engine::Camera::draw(const glm::vec3& _position)
{
    // to display a camera cube gizmo one day
}

void engine::Camera::updateCameraVectors()
{
}

void engine::Camera::setup()
{
}

bool engine::Camera::checkPlanes()
{
    if (m_nearPlane < MIN_CAMERA_NEAR_PLANE)
    {
        logger.warn("Camera {} near plane is invalid", m_name);
        return false;
    }

    if (m_farPlane > MAX_CAMERA_FAR_PLANE)
    {
        logger.warn("Camera {} far plane is invalid", m_name);
        return false;
    }

    if (m_farPlane < m_nearPlane)
    {
        logger.warn("Camera {} near/far planes are invalid", m_name);
        return false;
    }

    float farNearRatio = m_farPlane / m_nearPlane;
    if (farNearRatio > 100000)
    {
        logger.warn("Camera {} dangerous far/near planes ratio, possible z-fighting issues", m_name);
        return false;
    }

    return true;
}


// https://learnopengl.com/code_viewer_gh.php?code=src/8.guest/2021/1.scene/2.frustum_culling/frustum_culling.cpp
engine::Frustum engine::Camera::createFrustumFromCamera(float aspect, float fovY, float zNear, float zFar)
{
    // check near and far planes are valid
    checkPlanes();

    engine::Frustum frustum;

    if (m_enabled)
    {
        const float halfVSide = zFar * tanf(fovY * .5f);
        const float halfHSide = halfVSide * aspect;
        const glm::vec3 frontMultFar = zFar * front;

        frustum.nearFace = { position + zNear * front, front };
        frustum.farFace = { position + frontMultFar, -front };

        frustum.rightFace = { position, glm::cross(frontMultFar - right * halfHSide, up) };
        frustum.leftFace = { position, glm::cross(up, frontMultFar + right * halfHSide) };
        frustum.topFace = { position, glm::cross(right, frontMultFar - up * halfVSide) };
        frustum.bottomFace = { position, glm::cross(frontMultFar + up * halfVSide, right) };

        // Normalize all normals
        frustum.nearFace.normal = glm::normalize(frustum.nearFace.normal);
        frustum.farFace.normal = glm::normalize(frustum.farFace.normal);
        frustum.rightFace.normal = glm::normalize(frustum.rightFace.normal);
        frustum.leftFace.normal = glm::normalize(frustum.leftFace.normal);
        frustum.topFace.normal = glm::normalize(frustum.topFace.normal);
        frustum.bottomFace.normal = glm::normalize(frustum.bottomFace.normal);
    }

    return frustum;
}

engine::Bounds2D engine::Camera::getBounds(float aspect, float fovY, float zNear)
{
    Bounds2D bounds{ 0.0f, 0.0f };

    if (!m_enabled) {
        return bounds;
    }

    // Calculate half-height and half-width of the near plane
    const float halfVSide = zNear * tanf(glm::radians(fovY) * 0.5f);
    const float halfHSide = halfVSide * aspect;

    // Total width and height of the near plane
    bounds.width = 2.0f * halfHSide;
    bounds.height = 2.0f * halfVSide;

    return bounds;
}

engine::Camera::~Camera()
{
    logger.trace("Camera base destructor called");
}