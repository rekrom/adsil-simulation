#include <spatial/implementations/Transform.hpp>
#include <cmath>

namespace spatial
{
    Transform::Transform()
        : orientation_(0.0F, 0.0F, 0.0F)
    {
    }

    Transform::Transform(Point position, Vector orientation)
        : position_(std::move(position)), orientation_(std::move(orientation)) {}

    const Point &Transform::getPosition() const
    {
        return position_;
    }

    void Transform::setPosition(const Point &position)
    {
        position_ = position;
    }

    const Vector &Transform::getOrientation() const
    {
        return orientation_;
    }

    void Transform::setOrientation(const Vector &orientation)
    {
        orientation_ = orientation;
    }

    void Transform::move(const Vector &delta)
    {
        // position_ = Point(position_.x() + delta.x(),
        //                   position_.y() + delta.y(),
        //                   position_.z() + delta.z());
        position_ = position_ + delta;
    }

    Vector Transform::get3DDirectionVector() const
    {
        // float roll = orientation_.x();
        float pitch = orientation_.y();
        float yaw = orientation_.z();

        // float cRoll = std::cos(roll);
        // float sRoll = std::sin(roll);

        float cPitch = std::cos(pitch);
        float sPitch = std::sin(pitch);

        float cYaw = std::cos(yaw);
        float sYaw = std::sin(yaw);

        // Dönüş matrisi elemanları (Z-Y-X sırayla Euler açılarından)
        float m00 = cYaw * cPitch;
        // float m01 = cYaw * sPitch * sRoll - sYaw * cRoll;
        // float m02 = cYaw * sPitch * cRoll + sYaw * sRoll;

        float m10 = sYaw * cPitch;
        // float m11 = sYaw * sPitch * sRoll + cYaw * cRoll;
        // float m12 = sYaw * sPitch * cRoll - cYaw * sRoll;

        float m20 = -sPitch;
        // float m21 = cPitch * sRoll;
        // float m22 = cPitch * cRoll;

        // İleri yön vektörü lokal (1, 0, 0)
        // Dünya koordinatlarına dönüşüm:
        float x = m00; // 1 * m00 + 0 * m01 + 0 * m02
        float y = m10; // 1 * m10 + 0 * m11 + 0 * m12
        float z = m20; // 1 * m20 + 0 * m21 + 0 * m22

        Vector dir(x, y, z);
        return dir.normalized();
    }

    void Transform::set3DDirectionVector(const Vector &dir)
    {
        Vector normDir = dir.normalized();

        // Assumption: local forward is (0, 0, 1), we're reversing the get3DDirectionVector logic.
        // We aim to find the pitch and yaw that result in `normDir` when applying rotation.
        // Roll is assumed to be 0.

        float yaw = std::atan2(normDir.x(), normDir.z()); /// forward is (0, 0, 1)
                                                          // around Z

        // Pitch (around X axis): angle from horizontal plane to direction
        float pitch = std::atan2(-normDir.y(), std::sqrt(normDir.x() * normDir.x() + normDir.z() * normDir.z()));

        float roll = 0.0F; // default, unless banking is needed

        orientation_ = Vector(roll, pitch, yaw);
    }

    void Transform::rotateYaw(float angleRad)
    {
        Vector ori = orientation_;
        ori = ori + Vector(0.F, 0.F, angleRad); // ✅ modifying Z (yaw = Z)
        orientation_ = ori;
    }

    void Transform::rotateYawPitchRoll(float yaw, float pitch, float roll)
    {
        Vector ori = orientation_;
        ori = ori + Vector(roll, pitch, yaw); // ✅ modifying X (roll
        // ✅ modifying Y (pitch), ✅ modifying Z (yaw)
        orientation_ = ori;
    }

    Transform Transform::operator*(const Transform &other) const
    {
        // Compose positions and orientations:
        Point newPos = this->getPosition() + this->getOrientation().rotatePoint(other.getPosition());
        Vector newOri = this->getOrientation() * other.getOrientation(); // Quaternion or equivalent

        return {newPos, newOri};
    }

    glm::mat4 Transform::getModelMatrix() const
    {
        glm::mat4 m(1.0F);
        m = glm::translate(m, position_.toGlmVec3());
        m *= glm::toMat4(orientation_.toGlmQuat());
        return m;
    }
}