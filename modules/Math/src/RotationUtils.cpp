#include <math/RotationUtils.hpp>
#include <cmath>

namespace math
{
    Vector RotationUtils::rotateRPY(const Vector &vec, const Vector &rpy)
    {
        float roll = rpy.x(), pitch = rpy.y(), yaw = rpy.z();

        // Yaw (Z)
        float x1 = vec.x() * std::cos(yaw) - vec.y() * std::sin(yaw);
        float y1 = vec.x() * std::sin(yaw) + vec.y() * std::cos(yaw);
        float z1 = vec.z();

        // Pitch (Y)
        float x2 = x1 * std::cos(pitch) + z1 * std::sin(pitch);
        float y2 = y1;
        float z2 = -x1 * std::sin(pitch) + z1 * std::cos(pitch);

        // Roll (X)
        float x3 = x2;
        float y3 = y2 * std::cos(roll) - z2 * std::sin(roll);
        float z3 = y2 * std::sin(roll) + z2 * std::cos(roll);

        return Vector(x3, y3, z3);
    }

    Vector RotationUtils::eulerFromDirection(const Vector &dir)
    {
        Vector normDir = dir.normalized();

        // Yaw: rotation around Y axis (Z → X)
        float yaw = std::atan2(normDir.x(), normDir.z());

        // Pitch: rotation around X axis (horizontal plane → Y)
        float pitch = std::atan2(-normDir.y(), std::sqrt(normDir.x() * normDir.x() + normDir.z() * normDir.z()));

        // Roll is assumed zero
        return Vector(0.0F, pitch, yaw); // roll, pitch, yaw
    }
}