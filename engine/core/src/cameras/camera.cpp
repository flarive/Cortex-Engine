#include "../../include/cameras/camera.h"


#include <glm/glm.hpp> //glm::mat4
#include <list> //std::list
#include <array> //std::array
#include <memory> //std::unique_ptr

engine::Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _zoom, float _yaw, float _pitch, float _speed, float _sensitivity)
	: front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(_speed), mouseSensitivity(_sensitivity), m_zoom(_zoom), yaw(_yaw), pitch(_pitch)
{
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

// https://learnopengl.com/code_viewer_gh.php?code=src/8.guest/2021/1.scene/2.frustum_culling/frustum_culling.cpp
engine::Frustum engine::Camera::createFrustumFromCamera(float aspect, float fovY, float zNear, float zFar)
{
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

//engine::FrustumCorners engine::Camera::getBounds(float aspect, float fovY, float zNear, float zFar)
//{
//    engine::FrustumCorners frustumCorners;
//
//    if (!m_enabled) {
//        // Return zeroed corners if camera is disabled
//        for (auto& corner : frustumCorners.corners) {
//            corner = glm::vec3(0.0f);
//        }
//        return frustumCorners;
//    }
//
//    // Compute half dimensions of the near and far planes
//    const float halfVSideNear = zNear * tanf(fovY * 0.5f);
//    const float halfHSideNear = halfVSideNear * aspect;
//    const float halfVSideFar = zFar * tanf(fovY * 0.5f);
//    const float halfHSideFar = halfVSideFar * aspect;
//
//    // Compute the center of the near and far planes
//    glm::vec3 frontNear = position + front * zNear;
//    glm::vec3 frontFar = position + front * zFar;
//
//    // Compute the 4 corners of the near and far planes
//    glm::vec3 rightNear = right * halfHSideNear;
//    glm::vec3 upNear = up * halfVSideNear;
//    glm::vec3 rightFar = right * halfHSideFar;
//    glm::vec3 upFar = up * halfVSideFar;
//
//    // Near plane corners
//    frustumCorners.corners[0] = frontNear - rightNear - upNear; // Bottom-left
//    frustumCorners.corners[1] = frontNear + rightNear - upNear; // Bottom-right
//    frustumCorners.corners[2] = frontNear + rightNear + upNear; // Top-right
//    frustumCorners.corners[3] = frontNear - rightNear + upNear; // Top-left
//
//    // Far plane corners
//    frustumCorners.corners[4] = frontFar - rightFar - upFar; // Bottom-left
//    frustumCorners.corners[5] = frontFar + rightFar - upFar; // Bottom-right
//    frustumCorners.corners[6] = frontFar + rightFar + upFar; // Top-right
//    frustumCorners.corners[7] = frontFar - rightFar + upFar; // Top-left
//
//    return frustumCorners;
//}

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

