#pragma once

#include <geometry/interfaces/IShape.hpp>
#include <geometry/implementations/ShapeBase.hpp>

#include <spatial/implementations/Transform.hpp>
#include <core/PointCloud.hpp>

class Cube : public ShapeBase
{
public:
    Cube(const Transform &transform, float dimension);

    std::shared_ptr<PointCloud> surfaceMesh(int quality = 16) const override;

    std::vector<Point> wireframe() const override;

    std::string toString() const override;

private:
    float dimension_;
    std::vector<Point> generateFace(
        const Vector &center,
        const Vector &u,
        const Vector &v,
        int n) const;
};
