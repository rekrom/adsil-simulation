#pragma once

#include <geometry/interfaces/IShape.hpp>
#include <geometry/configs/CubeConfig.hpp>
#include <geometry/configs/CylinderConfig.hpp>
#include <memory>

class ShapeFactory
{
public:
    static std::shared_ptr<IShape> createCube(const CubeConfig &config);
    static std::shared_ptr<IShape> createCylinder(const CylinderConfig &config);
};
