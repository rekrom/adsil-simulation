#pragma once

namespace spatial
{

    class IMovable
    {
    public:
        virtual ~IMovable() = default;

        /// Move the object forward based on internal speed and delta time
        virtual void moveForward(float deltaTime) = 0;

        /// Rotate the object around its yaw (Z) axis based on delta time
        virtual void rotateYaw(float deltaTime) = 0;

        virtual void setSpeed(float speed) = 0;
        [[nodiscard]] virtual float getSpeed() const = 0;
    };

} // namespace spatial
