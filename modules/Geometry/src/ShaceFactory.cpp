#include <geometry/factories/ShapeFactory.hpp>
#include <geometry/implementations/ShapeBase.hpp>
#include <geometry/implementations/Cube.hpp>
#include <geometry/implementations/Cylinder.hpp>
#include <spatial/implementations/Transform.hpp>

std::shared_ptr<ShapeBase> ShapeFactory::createCube(const CubeConfig &config)
{
    return std::make_shared<Cube>(config);
}

std::shared_ptr<ShapeBase> ShapeFactory::createCylinder(const CylinderConfig &config)
{
    return std::make_shared<Cylinder>(config);
}
