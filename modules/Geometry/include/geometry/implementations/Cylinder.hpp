#pragma once

#include <geometry/implementations/ShapeBase.hpp>
#include <geometry/configs/CylinderConfig.hpp>
#include <math/Point.hpp>
#include <math/PointCloud.hpp>
#include <math/Vector.hpp>

class Cylinder : public ShapeBase
{
public:
    Cylinder(CylinderConfig config);

    std::shared_ptr<PointCloud> surfaceMesh(int quality = 2048) const override;

    std::vector<Point> wireframe() const override;

    std::string toString() const override;

    float getRadius();
    float getHeight();

private:
    CylinderDimension cylinderDimension;
};
