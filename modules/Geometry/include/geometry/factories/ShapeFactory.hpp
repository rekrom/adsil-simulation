#pragma once

#include <geometry/interfaces/IShape.hpp>
#include <geometry/implementations/ShapeBase.hpp>
#include <geometry/configs/CubeConfig.hpp>
#include <geometry/configs/CylinderConfig.hpp>
#include <memory>

class ShapeFactory
{
public:
    static std::shared_ptr<ShapeBase> createCube(const CubeConfig &config);
    static std::shared_ptr<ShapeBase> createCylinder(const CylinderConfig &config);
};
