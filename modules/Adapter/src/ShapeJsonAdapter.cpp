#include <adapter/implementations/ShapeJsonAdapter.hpp>
#include <geometry/implementations/Cube.hpp>
#include <geometry/implementations/Cylinder.hpp>
#include <geometry/factories/ShapeFactory.hpp>
#include <geometry/configs/CubeConfig.hpp>
#include <geometry/configs/CylinderConfig.hpp>

#include <nlohmann/json.hpp>

namespace adapter
{

    nlohmann::json ShapeJsonAdapter::toJson(const std::shared_ptr<IShape> &shape) const
    {
        nlohmann::json j;

        if (auto cube = std::dynamic_pointer_cast<Cube>(shape))
        {
            j["type"] = "Cube";
            j["origin"] = {cube->getOrigin().x(), cube->getOrigin().y(), cube->getOrigin().z()};
            j["orientation"] = {cube->getOrientation().x(), cube->getOrientation().y(), cube->getOrientation().z()};
            j["dimension"] = cube->getDimension().height;
        }
        else if (auto cylinder = std::dynamic_pointer_cast<Cylinder>(shape))
        {
            j["type"] = "Cylinder";
            j["origin"] = {cylinder->getOrigin().x(), cylinder->getOrigin().y(), cylinder->getOrigin().z()};
            j["orientation"] = {cylinder->getOrientation().x(), cylinder->getOrientation().y(), cylinder->getOrientation().z()};
            j["height"] = cylinder->getHeight();
            j["radius"] = cylinder->getRadius();
        }
        else
        {
            throw std::runtime_error("Unsupported shape type in toJson()");
        }

        return j;
    }

    std::shared_ptr<IShape> ShapeJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        const std::string type = j.at("type").get<std::string>();

        Point origin(
            j.at("origin")[0].get<float>(),
            j.at("origin")[1].get<float>(),
            j.at("origin")[2].get<float>());

        Vector orientation(
            j.at("orientation")[0].get<float>(),
            j.at("orientation")[1].get<float>(),
            j.at("orientation")[2].get<float>());

        if (type == "Cube")
        {
            CubeConfig config{
                .origin = origin,
                .dimension = j.at("dimension").get<float>(),
                .orientation = orientation,
            };
            return ShapeFactory::createCube(config);
        }
        else if (type == "Cylinder")
        {
            CylinderConfig config{
                .origin = origin,
                .height = j.at("height").get<float>(),
                .radius = j.at("radius").get<float>(),
                .orientation = orientation,
            };
            return ShapeFactory::createCylinder(config);
        }

        throw std::runtime_error("Unknown shape type in fromJson()");
    }

}
