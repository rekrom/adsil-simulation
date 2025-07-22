#pragma once

#include <geometry/interfaces/IShape.hpp>
#include <geometry/implementations/ShapeBase.hpp>
#include <geometry/configs/CubeConfig.hpp>

#include <spatial/implementations/Transform.hpp>
#include <math/PointCloud.hpp>

class Cube : public ShapeBase
{
public:
    Cube(CubeConfig config);

    std::shared_ptr<math::PointCloud> surfaceMesh(int quality = 2048) const override;

    std::vector<math::Point> wireframe() const override;

    std::string toString() const override;

    CubeDimension getDimension();

private:
    CubeDimension cubeDimension_;
    std::vector<math::Point> generateFace(
        const Vector &center,
        const Vector &u,
        const Vector &v,
        int n) const;
};
