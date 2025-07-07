#include "main.hpp"
#include <iostream>
#include <filesystem>

int main()
{
    // adapter initializations
    std::cout << "Current path is " << std::filesystem::current_path() << std::endl;
    adapter::AdapterManager adapters;
    auto car = adapters.fromJson<std::shared_ptr<Car>>("bin/resources/car.json");
    auto scene = adapters.fromJson<std::shared_ptr<SimulationScene>>("bin/resources/objects.json");

    // // SignalSolver solver(scene);
    // // auto closestPoints = solver.solve();
    // // std::cout << closestPoints->toString() << std::endl;

    // // auto closestPointRenderable = std::make_shared<PointCloudRenderable>(closestPoints);
    // // closestPointRenderable->setColor(glm::vec3(1.0f, 0.0f, 0.5f));
    // // closestPointRenderable->setVisible(true);
    // // viewer.addRenderable(closestPointRenderable);

    // // VIEWER START
    viewer::OpenGLViewer viewer(1280, 720, "ADSIL Analyzer - OpenGL");

    SharedVec<viewer::Entity> entities;

    auto axisEntity = std::make_shared<viewer::AxisEntity>();
    viewer.addEntity(axisEntity); // Assuming `addEntity` works on IEntity*

    auto groundEntity = std::make_shared<viewer::GroundEntity>();
    viewer.addEntity(groundEntity); // Assuming `addEntity` works on IEntity*

    auto carEntity = std::make_shared<viewer::CarEntity>(car, glm::vec3(0.2f, 0.6f, 0.9f));
    viewer.addEntity(carEntity);

    std::cout << "[INFO] Scene loaded." << std::endl;

    for (auto shape : scene->getShapes())
    {
        auto shapeEntity = std::make_shared<viewer::ShapeEntity>(shape);
        viewer.addEntity(shapeEntity);
        // entities.push_back(shapeEntity);
    }

    // entities.push_back(axisEntity);
    // entities.push_back(groundEntity);
    // entities.push_back(carEntity);

    // for (auto entity : entities)
    // {
    //     std::cout << entity->getName() << std::endl;
    // }

    std::cout << "[INFO] Entities loaded." << std::endl;

    viewer.run();

    return 0;
}