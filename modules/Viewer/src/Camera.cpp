#include <viewer/interfaces/Camera.hpp>
#include <iostream>
#include <sstream>
#include <string>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position_(position), worldUp_(up), yaw_(yaw), pitch_(pitch), fov_(45.0F), isLocked_(false)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(fov_), aspectRatio, 0.1F, 1000.0F);
}

void Camera::processKeyboard(char direction, float deltaTime)
{
    if (direction == 'L')
    {
        isLocked_ = !isLocked_;
    }
    if (!isLocked_)
        return;
    float velocity = 10.0F * deltaTime;
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
        yaw_ -= 60.F * deltaTime;
        updateCameraVectors();
    }
    if (direction == 'E')
    {
        yaw_ += 60.F * deltaTime;
        updateCameraVectors();
    }
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
    float sensitivity = 0.1F;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw_ += xOffset;
    pitch_ += yOffset;

    pitch_ = std::clamp(pitch_, -89.0F, 89.0F);
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    fov_ -= yoffset;
    if (fov_ < 1.0F)
        fov_ = 1.0F;
    if (fov_ > 90.0F)
        fov_ = 90.0F;
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cosf(glm::radians(yaw_)) * cosf(glm::radians(pitch_));
    front.y = sinf(glm::radians(pitch_));
    front.z = sinf(glm::radians(yaw_)) * cosf(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}

glm::vec3 Camera::getPosition() const
{
    return position_;
}

bool Camera::getIsLocked() const
{
    return isLocked_;
}

void Camera::setIsLocked(bool isLocked)
{
    isLocked_ = isLocked;
}

float Camera::getFov() const
{
    return fov_;
}

std::string Camera::toString() const
{
    std::ostringstream oss;
    oss << "Camera {\n";
    oss << "  Position: (" << position_.x << ", " << position_.y << ", " << position_.z << ")\n";
    oss << "  Up: (" << up_.x << ", " << up_.y << ", " << up_.z << ")\n";
    oss << "  Yaw: " << yaw_ << "\n";
    oss << "  Pitch: " << pitch_ << "\n";
    oss << "}";
    return oss.str();
}
