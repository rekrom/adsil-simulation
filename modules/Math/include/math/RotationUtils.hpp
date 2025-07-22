#pragma once

#include "Vector.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp> // <-- Bunu ekle

namespace RotationUtils
{
    Vector rotateRPY(const Vector &vec, const Vector &rpy);
    inline glm::mat4 eulerToRotationMatrix(const Vector &rotation)
    {
        glm::mat4 rot = glm::mat4(1.0F);
        rot = glm::rotate(rot, rotation.z(), glm::vec3(0.0F, 0.0F, 1.0F));
        rot = glm::rotate(rot, rotation.y(), glm::vec3(0.0F, 1.0F, 0.0F));
        rot = glm::rotate(rot, rotation.x(), glm::vec3(1.0F, 0.0F, 0.0F));
        return rot;
    }

    template <typename T>
    inline constexpr T deg2rad(T deg)
    {
        return deg * static_cast<T>(M_PI) / static_cast<T>(180);
    }

    template <typename T>
    inline constexpr T rad2deg(T rad)
    {
        return rad * static_cast<T>(180) / static_cast<T>(M_PI);
    }

    Vector eulerFromDirection(const Vector &dir);

}
