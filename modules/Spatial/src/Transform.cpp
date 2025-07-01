#include <spatial/implementations/Transform.hpp>
#include <cmath>

Transform::Transform()
    : position_(), orientation_(0.0f, 0.0f, 0.0f)
{
}

Transform::Transform(const Point &position, const Vector &orientation)
    : position_(position), orientation_(orientation)
{
}

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

const Vector Transform::get3DDirectionVector() const
{
    float roll = orientation_.x();
    float pitch = orientation_.y();
    float yaw = orientation_.z();

    float cRoll = std::cos(roll);
    float sRoll = std::sin(roll);

    float cPitch = std::cos(pitch);
    float sPitch = std::sin(pitch);

    float cYaw = std::cos(yaw);
    float sYaw = std::sin(yaw);

    // Dönüş matrisi elemanları (Z-Y-X sırayla Euler açılarından)
    float m00 = cYaw * cPitch;
    float m01 = cYaw * sPitch * sRoll - sYaw * cRoll;
    float m02 = cYaw * sPitch * cRoll + sYaw * sRoll;

    float m10 = sYaw * cPitch;
    float m11 = sYaw * sPitch * sRoll + cYaw * cRoll;
    float m12 = sYaw * sPitch * cRoll - cYaw * sRoll;

    float m20 = -sPitch;
    float m21 = cPitch * sRoll;
    float m22 = cPitch * cRoll;

    // İleri yön vektörü lokal (0, 1, 0)
    // Dünya koordinatlarına dönüşüm:
    float x = m01; // 0 * m00 + 1 * m01 + 0 * m02
    float y = m11; // 0 * m10 + 1 * m11 + 0 * m12
    float z = m21; // 0 * m20 + 1 * m21 + 0 * m22

    Vector dir(x, y, z);
    return dir.normalized();
}

void Transform::set3DDirectionVector(const Vector &dir)
{
    Vector normDir = dir.normalized();

    // Assumption: local forward is (0, 1, 0), we're reversing the get3DDirectionVector logic.
    // We aim to find the pitch and yaw that result in `normDir` when applying rotation.
    // Roll is assumed to be 0.

    float yaw = std::atan2(normDir.x(), normDir.y());                                                         // around Z
    float pitch = std::atan2(-normDir.z(), std::sqrt(normDir.x() * normDir.x() + normDir.y() * normDir.y())); // around Y

    orientation_ = Vector(0.0f, pitch, yaw); // roll = 0
}

void Transform::rotateYaw(float angleRad)
{
    Vector ori = orientation_;
    ori = Vector(ori.x(), ori.y(), ori.z() + angleRad);
    orientation_ = ori;
}

Transform Transform::operator*(const Transform &other) const
{
    // Compose positions and orientations:
    Point newPos = this->getPosition() + this->getOrientation().rotatePoint(other.getPosition());
    Vector newOri = this->getOrientation() * other.getOrientation(); // Quaternion or equivalent

    return Transform(newPos, newOri);
}

glm::mat4 Transform::getModelMatrix() const
{
    glm::mat4 m(1.0f);
    m = glm::translate(m, position_.toGlmVec3());
    m *= glm::toMat4(orientation_.toGlmQuat());
    return m;
}