#include <viewer/interfaces/Camera.hpp>
#include <iostream>
#include <sstream>
#include <string>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position_(position), worldUp_(up), yaw_(yaw), pitch_(pitch), fov_(45.0F), isLocked_(true)
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
        yaw_ += glm::radians(60.F) * deltaTime;
        updateCameraVectors();
    }
    if (direction == 'E')
    {
        yaw_ -= glm::radians(60.F) * deltaTime;
        updateCameraVectors();
    }
    if (direction == 'Z')
    {
        pitch_ -= glm::radians(60.F) * deltaTime;
        updateCameraVectors();
    }
    if (direction == 'C')
    {
        pitch_ += glm::radians(60.F) * deltaTime;
        updateCameraVectors();
    }
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
    xOffset *= sensitivity_;
    yOffset *= sensitivity_;

    yaw_ -= xOffset;
    pitch_ -= yOffset;

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

// void Camera::updateCameraVectors()
// {
//     glm::vec3 front;
//     front.x = cosf(yaw_) * cosf(pitch_);
//     front.y = sinf(pitch_);
//     front.z = sinf(yaw_) * cosf(pitch_);
//     front_ = glm::normalize(front);
//     right_ = glm::normalize(glm::cross(front_, worldUp_));
//     up_ = glm::normalize(glm::cross(right_, front_));
// }

void Camera::updateCameraVectors()
{
    math::Vector local_x(1.0F, 0.0F, 0.0F); // Initial front vector
    math::Vector local_y(0.0F, 1.0F, 0.0F); // Initial left vector
    math::Vector local_z(0.0F, 0.0F, 1.0F); // Initial up vector

    // Lambda function for rotating a vector around an axis

    local_x = math::RotationUtils::rotateAroundAxis(local_x, local_z, yaw_);
    local_y = math::RotationUtils::rotateAroundAxis(local_y, local_z, yaw_);

    local_x = math::RotationUtils::rotateAroundAxis(local_x, local_y, pitch_);
    local_z = math::RotationUtils::rotateAroundAxis(local_z, local_y, pitch_);

    front_ = local_x.normalized().toGlmVec3();
    right_ = -local_y.normalized().toGlmVec3();
    up_ = local_z.normalized().toGlmVec3();
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

void Camera::setPitch(float pitch)
{
    pitch = glm::radians(pitch);
    updateCameraVectors();
}
void Camera::setYaw(float yaw)
{
    yaw = glm::radians(yaw);
    yaw_ = std::fmod(yaw, 360.0F);
    if (yaw > 180.0F)
        yaw -= 360.0F;
    updateCameraVectors();
}

float Camera::getPitch() const
{
    return glm::degrees(pitch_);
}
float Camera::getYaw() const
{
    return glm::degrees(yaw_);
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
