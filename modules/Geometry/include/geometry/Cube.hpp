#pragma once

#include <geometry/IShape.hpp>
#include <core/Point.hpp>
#include <core/Vector.hpp>

class Cube : public IShape
{
public:
    Cube(const Point &origin, float dimension, const Vector &orientation);

    std::shared_ptr<PointCloud> surfaceMesh(int quality = 16) const override;

    std::vector<Point> wireframe() const override;

    std::string toString() const override;

private:
    Point origin_;
    float dimension_;
    Vector orientation_;

    std::vector<Point> generateFace(
        const Vector &center,
        const Vector &u,
        const Vector &v,
        int n) const;
};
