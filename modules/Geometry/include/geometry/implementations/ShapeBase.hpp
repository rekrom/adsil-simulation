#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <spatial/implementations/Transform.hpp>
#include <geometry/interfaces/IShape.hpp>

class ShapeBase : public IShape
{
public:
    ShapeBase(const Transform &transform)
        : transform_(transform) {}

    const Transform &getTransform() const { return transform_; }
    void setTransform(const Transform &transform) { transform_ = transform; }

    const Point &getOrigin() const { return transform_.getPosition(); }
    const Vector &getOrientation() const { return transform_.getOrientation(); }

    void setOrigin(const Point &origin) { transform_.setPosition(origin); }
    void setOrientation(const Vector &orientation) { transform_.setOrientation(orientation); }

protected:
    Transform transform_;
};
