#pragma once

#include "IShape.hpp"
#include "Point.hpp"
#include "PointCloud.hpp"
#include "Vector.hpp"

class Cylinder : public IShape
{
public:
    Cylinder(const Point &origin, float height, float radius, const Vector &orientation);

    std::shared_ptr<PointCloud> surfaceMesh(int quality = 16) const override;
    std::string toString() const override;

private:
    Point origin_;
    float height_;
    float radius_;
    Vector orientation_;
};
