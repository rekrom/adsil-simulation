#include <adapter/implementations/ShapeJsonAdapter.hpp>
#include <geometry/implementations/Cube.hpp>
#include <geometry/implementations/Cylinder.hpp>
#include <geometry/factories/ShapeFactory.hpp>
#include <geometry/configs/CubeConfig.hpp>
#include <geometry/configs/CylinderConfig.hpp>

#include <nlohmann/json.hpp>

namespace adapter
{

    nlohmann::json ShapeJsonAdapter::toJson(const std::shared_ptr<ShapeBase> &shape) const
    {
        nlohmann::json j;

        if (auto cube = std::dynamic_pointer_cast<Cube>(shape))
        {
            j["type"] = "Cube";
            j["origin"] = {cube->getOrigin().x(), cube->getOrigin().y(), cube->getOrigin().z()};
            j["orientation"] = {cube->getOrientation().x(), cube->getOrientation().y(), cube->getOrientation().z()};
            j["dimension"] = cube->getDimension().height;
            j["name"] = cube->getName();
        }
        else if (auto cylinder = std::dynamic_pointer_cast<Cylinder>(shape))
        {
            j["type"] = "Cylinder";
            j["origin"] = {cylinder->getOrigin().x(), cylinder->getOrigin().y(), cylinder->getOrigin().z()};
            j["orientation"] = {cylinder->getOrientation().x(), cylinder->getOrientation().y(), cylinder->getOrientation().z()};
            j["height"] = cylinder->getHeight();
            j["radius"] = cylinder->getRadius();
            j["name"] = cylinder->getName();
        }
        else
        {
            throw std::runtime_error("Unsupported shape type in toJson()");
        }

        return j;
    }

    std::shared_ptr<ShapeBase> ShapeJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        const std::string type = j.at("type").get<std::string>();

        Point origin = pointAdapter_.fromJson(j.at("origin"));

        Vector v = vectorAdapter_.fromJson(j.at("orientation"));
        Vector orientation(RotationUtils::deg2rad(v.x()), RotationUtils::deg2rad(v.y()), RotationUtils::deg2rad(v.z()));

        if (type == "Cube")
        {
            CubeConfig config{
                .origin = origin,
                .orientation = orientation,
                .dimension = j.at("dimension").get<float>(),
                .name = j.at("name").get<std::string>()};
            return ShapeFactory::createCube(config);
        }
        else if (type == "Cylinder")
        {
            CylinderConfig config{
                .origin = origin,
                .orientation = orientation,
                .height = j.at("height").get<float>(),
                .radius = j.at("radius").get<float>(),
                .name = j.at("name").get<std::string>()

            };
            return ShapeFactory::createCylinder(config);
        }

        throw std::runtime_error("Unknown shape type in fromJson()");
    }

}
