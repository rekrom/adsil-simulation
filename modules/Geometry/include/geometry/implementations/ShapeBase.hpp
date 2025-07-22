#pragma once

#include <math/Point.hpp>
#include <math/Vector.hpp>
#include <spatial/implementations/Transform.hpp>
#include <spatial/implementations/HasTransform.hpp>
#include <geometry/interfaces/IShape.hpp>
#include <iostream>

class ShapeBase : public IShape, public spatial::HasTransform
{
public:
    ShapeBase(std::string name)
        : name_(name) {}

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
    std::string name_;
    mutable std::shared_ptr<PointCloud> surfaceMeshPcd_; //  since its using by a const method but it modifies a member.
    int meshQuality_{2048};
};
