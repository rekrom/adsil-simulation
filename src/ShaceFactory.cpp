#include "ShapeFactory.hpp"
#include "Cube.hpp"
#include "Cylinder.hpp"

std::shared_ptr<IShape> ShapeFactory::createCube(const CubeConfig &config)
{
    return std::make_shared<Cube>(config.origin, config.dimension, config.orientation);
}

std::shared_ptr<IShape> ShapeFactory::createCylinder(const CylinderConfig &config)
{
    return std::make_shared<Cylinder>(config.origin, config.height, config.radius, config.orientation);
}
