//#include "../include/camera.h"
//
//engine::Camera::Camera(glm::vec3 _position, bool _fps, glm::vec3 _up, float _yaw, float _pitch)
//    : position(_position), fps(_fps), front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)//, worldUp(_up), yaw(_yaw), pitch(_pitch)
//{
//    position = _position;
//    worldUp = _up;
//    yaw = _yaw;
//    pitch = _pitch;
//    updateCameraVectors();
//}
//
//// constructor with scalar values
//engine::Camera::Camera(float _posX, float _posY, float _posZ, float _upX, float _upY, float _upZ, float _yaw, float _pitch, bool _fps)
//    : position(glm::vec3(_posX, _posY, _posZ)), front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), fps(_fps)//, worldUp(glm::vec3(_upX, _upY, _upZ)), yaw(_yaw), pitch(_pitch)
//{
//    position = glm::vec3(_posX, _posY, _posZ);
//    worldUp = glm::vec3(_upX, _upY, _upZ);
//    yaw = _yaw;
//    pitch = _pitch;
//    updateCameraVectors();
//}
//
//glm::mat4 engine::Camera::GetViewMatrix()
//{
//    return glm::lookAt(position, position + front, up);
//}
//
//// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
//void engine::Camera::ProcessKeyboard(engine::Camera_Movement direction, float deltaTime, GLboolean constrainPitch)
//{
//    float velocity = movementSpeed * deltaTime;
//    if (direction == FORWARD)
//        position += front * velocity;
//    if (direction == BACKWARD)
//        position -= front * velocity;
//    if (direction == LEFT)
//        position -= right * velocity;
//    if (direction == RIGHT)
//        position += right * velocity;
//    if (direction == UP)
//        position += up * velocity;
//    if (direction == DOWN)
//        position -= up * velocity;
//
//    if (direction == YAW_UP)
//        yaw += 20 * velocity;
//    if (direction == YAW_DOWN)
//        yaw -= 20 * velocity;
//
//    if (direction == PITCH_UP)
//        pitch += 20 * velocity;
//    if (direction == PITCH_DOWN)
//        pitch -= 20 * velocity;
//
//    // make sure that when pitch is out of bounds, screen doesn't get flipped
//    if (constrainPitch)
//    {
//        if (pitch > 89.0f)
//            pitch = 89.0f;
//        if (pitch < -89.0f)
//            pitch = -89.0f;
//    }
//
//    // for First Person Shooter camera
//    if (fps)
//        position.y = 0.0f; // <-- this one-liner keeps the user at the ground level (xz plane)
//
//    // update Front, Right and Up Vectors using the updated Euler angles
//    updateCameraVectors();
//}
//
//// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
//void engine::Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
//{
//    xoffset *= mouseSensitivity;
//
//    if (fps)
//        yoffset = 0;
//    else
//        yoffset *= mouseSensitivity;
//
//
//    yaw += xoffset;
//    pitch += yoffset;
//
//    // make sure that when pitch is out of bounds, screen doesn't get flipped
//    if (constrainPitch)
//    {
//        if (pitch > 89.0f)
//            pitch = 89.0f;
//        if (pitch < -89.0f)
//            pitch = -89.0f;
//    }
//
//    // update Front, Right and Up Vectors using the updated Euler angles
//    updateCameraVectors();
//}
//
//void engine::Camera::ProcessJoystickMovement(const GLFWgamepadstate& state)
//{
//    float deadZone = 0.2f; // Dead zone threshold to prevent drift
//
//    float leftX = (fabs(state.axes[0]) > deadZone) ? state.axes[0] : 0.0f; // Left stick X-axis (left/right movement)
//    float leftY = (fabs(state.axes[1]) > deadZone) ? state.axes[1] : 0.0f; // Left stick Y-axis (forward/backward movement)
//    float rightX = (fabs(state.axes[2]) > deadZone) ? state.axes[2] : 0.0f; // Right stick X-axis (yaw rotation)
//    float rightY = (fabs(state.axes[3]) > deadZone) ? state.axes[3] : 0.0f; // Right stick Y-axis (pitch rotation)
//    float triggerL = (state.axes[4] > -0.9f) ? state.axes[4] : -1.0f; // Left trigger (down movement)
//    float triggerR = (state.axes[5] > -0.9f) ? state.axes[5] : -1.0f; // Right trigger (up movement)
//
//    float velocity = movementSpeed * 0.1f; // Adjust movement speed
//    float rotationSpeed = mouseSensitivity * 2.0f; // Adjust rotation sensitivity
//
//    // Apply movement (left stick)
//    position += front * (-leftY * velocity); // Forward/backward
//    position += right * (leftX * velocity); // Left/right
//
//    // Vertical movement using triggers
//    if (triggerL > -0.9f) position -= up * ((triggerL + 1.0f) * 0.5f * velocity); // L2 moves down
//    if (triggerR > -0.9f) position += up * ((triggerR + 1.0f) * 0.5f * velocity); // R2 moves up
//
//    // Apply camera rotation (right stick)
//    yaw += rightX * rotationSpeed;
//    pitch -= rightY * rotationSpeed;
//
//    // Clamp pitch to avoid flipping
//    if (pitch > 89.0f) pitch = 89.0f;
//    if (pitch < -89.0f) pitch = -89.0f;
//
//    // Update camera vectors
//    updateCameraVectors();
//}
//
//// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
//void engine::Camera::ProcessMouseScroll(float yoffset)
//{
//    zoom -= (float)yoffset;
//    if (zoom < 1.0f)
//        zoom = 1.0f;
//    if (zoom > 45.0f)
//        zoom = 45.0f;
//}
//
//void engine::Camera::updateCameraVectors()
//{
//    // calculate the new Front vector
//    glm::vec3 front;
//    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//    front.y = sin(glm::radians(pitch));
//    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//    front = glm::normalize(front);
//    // also re-calculate the Right and Up vector
//    right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//    up = glm::normalize(glm::cross(right, front));
//}


#include "../include/camera.h"

engine::Camera::Camera(glm::vec3 position, bool fps, glm::vec3 up, float yaw, float pitch)
    : Fps(fps), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// constructor with scalar values
engine::Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, bool fps)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), Fps(fps)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

glm::mat4 engine::Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void engine::Camera::ProcessKeyboard(engine::Camera_Movement direction, float deltaTime, GLboolean constrainPitch)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if (direction == UP)
        Position += Up * velocity;
    if (direction == DOWN)
        Position -= Up * velocity;

    if (direction == YAW_UP)
        Yaw += 20 * velocity;
    if (direction == YAW_DOWN)
        Yaw -= 20 * velocity;

    if (direction == PITCH_UP)
        Pitch += 20 * velocity;
    if (direction == PITCH_DOWN)
        Pitch -= 20 * velocity;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // for FPS camera
    if (Fps)
        Position.y = 0.0f; // <-- this one-liner keeps the user at the ground level (xz plane)

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void engine::Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;

    if (Fps)
        yoffset = 0;
    else
        yoffset *= MouseSensitivity;


    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void engine::Camera::ProcessJoystickMovement(const GLFWgamepadstate& state)
{
    float deadZone = 0.2f; // Dead zone threshold to prevent drift

    float leftX = (fabs(state.axes[0]) > deadZone) ? state.axes[0] : 0.0f; // Left stick X-axis (left/right movement)
    float leftY = (fabs(state.axes[1]) > deadZone) ? state.axes[1] : 0.0f; // Left stick Y-axis (forward/backward movement)
    float rightX = (fabs(state.axes[2]) > deadZone) ? state.axes[2] : 0.0f; // Right stick X-axis (yaw rotation)
    float rightY = (fabs(state.axes[3]) > deadZone) ? state.axes[3] : 0.0f; // Right stick Y-axis (pitch rotation)
    float triggerL = (state.axes[4] > -0.9f) ? state.axes[4] : -1.0f; // Left trigger (down movement)
    float triggerR = (state.axes[5] > -0.9f) ? state.axes[5] : -1.0f; // Right trigger (up movement)

    float velocity = MovementSpeed * 0.1f; // Adjust movement speed
    float rotationSpeed = MouseSensitivity * 2.0f; // Adjust rotation sensitivity

    // Apply movement (left stick)
    Position += Front * (-leftY * velocity); // Forward/backward
    Position += Right * (leftX * velocity); // Left/right

    // Vertical movement using triggers
    if (triggerL > -0.9f) Position -= Up * ((triggerL + 1.0f) * 0.5f * velocity); // L2 moves down
    if (triggerR > -0.9f) Position += Up * ((triggerR + 1.0f) * 0.5f * velocity); // R2 moves up

    // Apply camera rotation (right stick)
    Yaw += rightX * rotationSpeed;
    Pitch -= rightY * rotationSpeed;

    // Clamp pitch to avoid flipping
    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    // Update camera vectors
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void engine::Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void engine::Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}