#pragma once
#include <glm/vec3.hpp>
#include <core/Vector.hpp>

namespace spatial
{

    class IMovable
    {
    public:
        virtual ~IMovable() = default;

        /// Move the object to a new position
        virtual void moveBy(const Vector &delta) = 0;

        /// Move the object forward based on internal speed and delta time
        virtual void moveForward(float deltaTime) = 0;

        /// Rotate the object around its yaw (Z) axis based on delta time
        virtual void rotateYaw(float deltaTime) = 0;

        /// Rotate the object by a given yaw, pitch, and roll in degrees
        virtual void rotateByYawPitchRoll(float yaw, float pitch, float roll) = 0;

        virtual void setSpeed(float speed) = 0;
        [[nodiscard]] virtual float getSpeed() const = 0;
    };

} // namespace spatial
