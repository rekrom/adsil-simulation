#include <geometry/factories/ShapeFactory.hpp>
#include <geometry/implementations/Cube.hpp>
#include <geometry/implementations/Cylinder.hpp>
#include <spatial/implementations/Transform.hpp>

std::shared_ptr<IShape> ShapeFactory::createCube(const CubeConfig &config)
{
    Transform transform(config.origin, config.orientation);
    return std::make_shared<Cube>(transform, config.dimension);
}

std::shared_ptr<IShape> ShapeFactory::createCylinder(const CylinderConfig &config)
{
    Transform transform(config.origin, config.orientation);

    // return std::make_shared<Cylinder>(transform, config.height, config.radius);
    return std::make_shared<Cylinder>(config.origin, config.height, config.radius, config.orientation);
}
