#include <adapter/implementations/SceneJsonAdapter.hpp>
#include <iostream>

namespace adapter
{
    SceneJsonAdapter::SceneJsonAdapter()
        : carAdapter_(), pointAdapter_(), shapeAdapter_()
    {
    }

    std::shared_ptr<SimulationScene> SceneJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        std::shared_ptr<SimulationScene> scene = std::make_shared<SimulationScene>();

        // Car
        if (j.contains("car"))
        {
            auto car = carAdapter_.fromJson(j.at("car"));
            scene->setCar(car);
        }

        if (j.contains("cubes"))
        {
            for (const auto &cubeJsonOriginal : j.at("cubes"))
            {
                auto cubeJson = cubeJsonOriginal;
                cubeJson["type"] = "Cube";
                auto shape = shapeAdapter_.fromJson(cubeJson);
                scene->addShape(shape);
            }
        }

        if (j.contains("cylinders"))
        {
            for (const auto &cylJsonOriginal : j.at("cylinders"))
            {
                auto cylJson = cylJsonOriginal;
                cylJson["type"] = "Cylinder";
                auto shape = shapeAdapter_.fromJson(cylJson);
                scene->addShape(shape);
            }
        }

        return scene;
    }

    nlohmann::json SceneJsonAdapter::toJson(const std::shared_ptr<SimulationScene> &scene) const
    {
        nlohmann::json j;

        // Car
        if (scene->hasCar())
            j["car"] = carAdapter_.toJson(scene->getCar());

        // Shapes
        j["cubes"] = nlohmann::json::array();
        j["cylinders"] = nlohmann::json::array();

        for (const auto &shape : scene->getShapes())
        {
            auto shapeJson = shapeAdapter_.toJson(shape);
            if (shapeJson["type"] == "Cube")
                j["cubes"].push_back(shapeJson);
            else if (shapeJson["type"] == "Cylinder")
                j["cylinders"].push_back(shapeJson);
        }

        return j;
    }

}
