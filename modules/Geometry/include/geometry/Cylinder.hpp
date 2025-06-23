#pragma once

#include <geometry/IShape.hpp>
#include <core/Point.hpp>
#include <core/PointCloud.hpp>
#include <core/Vector.hpp>

class Cylinder : public IShape
{
public:
    Cylinder(const Point &origin, float height, float radius, const Vector &orientation);

    std::shared_ptr<PointCloud> surfaceMesh(int quality = 16) const override;
    std::vector<Point> wireframe() const override;

    std::string toString() const override;

private:
    Point origin_;
    float height_;
    float radius_;
    Vector orientation_;
};
