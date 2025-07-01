#include <viewer/interfaces/Camera.hpp>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position_(position), worldUp_(up), yaw_(yaw), pitch_(pitch), fov_(45.0f)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(fov_), aspectRatio, 0.1f, 1000.0f);
}

void Camera::processKeyboard(char direction, float deltaTime)
{
    float velocity = 10.0f * deltaTime;
    if (direction == 'W')
        position_ += front_ * velocity;
    if (direction == 'S')
        position_ -= front_ * velocity;
    if (direction == 'A')
        position_ -= right_ * velocity;
    if (direction == 'D')
        position_ += right_ * velocity;
    if (direction == 'Q')
    {
        yaw_ -= 60.f * deltaTime;
        updateCameraVectors();
    }
    if (direction == 'E')
    {
        yaw_ += 60.f * deltaTime;
        updateCameraVectors();
    }
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw_ += xOffset;
    pitch_ += yOffset;

    pitch_ = std::clamp(pitch_, -89.0f, 89.0f);
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    fov_ -= yoffset;
    if (fov_ < 1.0f)
        fov_ = 1.0f;
    if (fov_ > 90.0f)
        fov_ = 90.0f;
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}

glm::vec3 Camera::getPosition() const
{
    return position_;
}

float Camera::getFov() const
{
    return fov_;
}