#pragma once

#include "IShape.hpp"
#include "CubeConfig.hpp"
#include "CylinderConfig.hpp"
#include <memory>

class ShapeFactory
{
public:
    static std::shared_ptr<IShape> createCube(const CubeConfig &config);
    static std::shared_ptr<IShape> createCylinder(const CylinderConfig &config);
};
