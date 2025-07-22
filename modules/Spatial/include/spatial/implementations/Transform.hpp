#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace spatial
{
    class Transform
    {
    public:
        Transform();
        Transform(Point position, Vector orientation); // <<=== Güncellendi

        [[nodiscard]] const Point &getPosition() const;
        void setPosition(const Point &position);

        [[nodiscard]] const Vector &getOrientation() const;
        void setOrientation(const Vector &orientation);

        void move(const Vector &delta);

        [[nodiscard]] Vector get3DDirectionVector() const;
        void set3DDirectionVector(const Vector &dir);

        void rotateYaw(float angleRad);

        void rotateYawPitchRoll(float yaw, float pitch, float roll);

        Transform operator*(const Transform &other) const;

        [[nodiscard]] glm::mat4 getModelMatrix() const;

    private:
        Point position_;
        Vector orientation_; // roll (x), pitch (y), yaw (z) — radyan cinsinden
    };
}
