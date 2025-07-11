#pragma once

#include <core/Point.hpp>
#include <core/Vector.hpp>
#include <spatial/implementations/Transform.hpp>
#include <spatial/implementations/HasTransform.hpp>
#include <geometry/interfaces/IShape.hpp>
#include <iostream>

class ShapeBase : public IShape, public spatial::HasTransform
{
public:
    ShapeBase(const spatial::Transform &transform, std::string name)
        : transform_(transform), name_(name) {}

    const spatial::Transform &getTransform() const { return transform_; }
    void setTransform(const spatial::Transform &transform) { transform_ = transform; }

    const Point &getOrigin() const { return transform_.getPosition(); }
    const Vector &getOrientation() const { return transform_.getOrientation(); }

    void setOrigin(const Point &origin) { transform_.setPosition(origin); }
    void setOrientation(const Vector &orientation) { transform_.setOrientation(orientation); }

    std::shared_ptr<PointCloud> getSurfaceMeshPCD() const override
    {
        if (!surfaceMeshPcd_ || surfaceMeshPcd_->size() == 0)
        {
            surfaceMeshPcd_ = surfaceMesh(meshQuality_);
        }

        return surfaceMeshPcd_;
    }

    const std::string &getName() const
    {
        return name_;
    }
    void setName(const std::string &name) { name_ = name; }

protected:
    spatial::Transform transform_;
    std::string name_;
    mutable std::shared_ptr<PointCloud> surfaceMeshPcd_; //  since its using by a const method but it modifies a member.
    int meshQuality_{2048};
};
