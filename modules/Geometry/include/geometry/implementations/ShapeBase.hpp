#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <spatial/implementations/Transform.hpp>
#include <spatial/implementations/HasTransformNodeBase.hpp>
#include <geometry/interfaces/IShape.hpp>

class ShapeBase : public IShape, public spatial::HasTransformNodeBase
{
public:
    ShapeBase(const Transform &transform, std::string name)
        : transform_(transform), name_(name) {}

    const Transform &getTransform() const { return transform_; }
    void setTransform(const Transform &transform) { transform_ = transform; }

    const Point &getOrigin() const { return transform_.getPosition(); }
    const Vector &getOrientation() const { return transform_.getOrientation(); }

    void setOrigin(const Point &origin) { transform_.setPosition(origin); }
    void setOrientation(const Vector &orientation) { transform_.setOrientation(orientation); }

    const std::string &getName() const { return name_; }
    void setName(const std::string &name) { name_ = name; }

protected:
    Transform transform_;
    std::string name_;
};
