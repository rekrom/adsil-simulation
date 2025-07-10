#include <geometry/factories/ShapeFactory.hpp>
#include <geometry/implementations/ShapeBase.hpp>
#include <geometry/implementations/Cube.hpp>
#include <geometry/implementations/Cylinder.hpp>
#include <spatial/implementations/Transform.hpp>

std::shared_ptr<ShapeBase> ShapeFactory::createCube(const CubeConfig &config)
{
    spatial::Transform transform(config.origin, config.orientation);
    return std::make_shared<Cube>(transform, config.dimension, config.name);
}

std::shared_ptr<ShapeBase> ShapeFactory::createCylinder(const CylinderConfig &config)
{
    spatial::Transform transform(config.origin, config.orientation);

    return std::make_shared<Cylinder>(transform, config.height, config.radius, config.name);
}
