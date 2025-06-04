#include "../../include/cameras/fly_camera.h"


engine::FlyCamera::FlyCamera(glm::vec3 position, bool fps, glm::vec3 up, float yaw, float pitch)
    : engine::Camera(position, fps, up, yaw, pitch)
{
}

// constructor with scalar values
engine::FlyCamera::FlyCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, bool fps)
    : engine::Camera(posX, posY, posZ, upX, upY, upZ, yaw, pitch, fps)
{
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void engine::FlyCamera::processKeyboard(engine::Camera_Movement direction, float deltaTime, GLboolean constrainPitch)
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
void engine::FlyCamera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
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

void engine::FlyCamera::processJoystickMovement(const GLFWgamepadstate& state)
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
void engine::FlyCamera::processMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}