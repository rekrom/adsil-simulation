#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform
{
public:
    Transform();
    Transform(const Point &position, const Vector &orientation);

    const Point &getPosition() const;
    void setPosition(const Point &position);

    const Vector &getOrientation() const;
    void setOrientation(const Vector &orientation);

    void move(const Vector &delta);

    const Vector get3DDirectionVector() const;
    void set3DDirectionVector(const Vector &dir);

    void rotateYaw(float angleRad);

    Transform operator*(const Transform &other) const;

    glm::mat4 getModelMatrix() const;

private:
    Point position_;
    Vector orientation_; // roll (x), pitch (y), yaw (z) — radyan cinsinden
};
