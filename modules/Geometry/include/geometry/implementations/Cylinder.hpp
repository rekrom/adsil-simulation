#pragma once

#include <geometry/implementations/ShapeBase.hpp>
#include <geometry/configs/CylinderConfig.hpp>
#include <core/Point.hpp>
#include <core/PointCloud.hpp>
#include <core/Vector.hpp>

class Cylinder : public ShapeBase
{
public:
    Cylinder(const spatial::Transform &transform, float height, float radius, std::string name);

    std::shared_ptr<PointCloud> surfaceMesh(int quality = 2048) const override;
    std::vector<Point> wireframe() const override;

    std::string toString() const override;

    float getRadius();
    float getHeight();

private:
    CylinderDimension cylinderDimension;
};
