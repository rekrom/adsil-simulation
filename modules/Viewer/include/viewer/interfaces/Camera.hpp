#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> // Required for std::clamp
#include <iostream>
class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void processKeyboard(char direction, float deltaTime);
    void processMouseMovement(float xOffset, float yOffset);
    void processMouseScroll(float yOffset);

    glm::vec3 getPosition() const;
    bool getIsLocked() const;
    void setIsLocked(bool isLocked);
    float getFov() const;

    std::string toString() const;

private:
    void updateCameraVectors();

    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 worldUp_;

    float yaw_;
    float pitch_;
    float fov_;

    bool isLocked_;
};
