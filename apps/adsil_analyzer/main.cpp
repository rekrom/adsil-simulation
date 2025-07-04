#include "main.hpp"
#include <iostream>

int main()
{
    // adapter initializations
    adapter::JsonAdapterRegistry registry;
    // register the carjson into the adapter
    registry.registerAdapter<std::shared_ptr<Car>>(std::make_shared<adapter::CarJsonAdapter>());
    // registry.registerAdapter<std::shared_ptr<IShape>>(std::make_shared<adapter::ShapeJsonAdapter>());
    registry.registerAdapter<std::shared_ptr<SimulationScene>>(std::make_shared<adapter::SceneJsonAdapter>());

    nlohmann::json car_json;
    if (!utils::readJsonFromFile("/home/rkrm-dev/Desktop/adsil_analyzer_cpp/resources/car.json", car_json))
    {
        exit(0);
    }

    nlohmann::json objects_json;
    if (!utils::readJsonFromFile("/home/rkrm-dev/Desktop/adsil_analyzer_cpp/resources/objects.json", objects_json))
    {
        exit(0);
    }

    auto *carAdapter = registry.getAdapter<std::shared_ptr<Car>>();
    if (!carAdapter)
    {
        std::cerr << "Car adapter bulunamadı!\n";
        return 1;
    }

    // auto *shapeAdapter = registry.getAdapter<std::shared_ptr<IShape>>();
    // if (!shapeAdapter)
    // {
    //     std::cerr << "IShape adapter bulunamadı!\n";
    //     return 1;
    // }

    auto *sceneAdapter = registry.getAdapter<std::shared_ptr<SimulationScene>>();
    if (!sceneAdapter)
    {
        std::cerr << "SimulationScene adapter bulunamadı!\n";
        return 1;
    }

    // auto cylinder = ShapeFactory::createCylinder({Point(-500, 100, 0),
    //                                               2000.0,
    //                                               300.0,
    //                                               Vector(0, 0, 0)});

    // // std::cout << cube->toString() << "\n";
    // // std::cout << cylinder->toString() << "\n";

    // std::cout << mesh->toString() << "\n";

    // // auto meshCy = cylinder->surfaceMesh(16);
    // // std::cout << meshCy->toString() << "\n";

    // for (const auto &tx : transmitters)
    // {
    //     auto txRenderable = std::make_shared<DeviceRenderable>(tx, glm::vec3(1.0f, 0.0f, 0.0f)); // kırmızı
    //     viewer.addRenderable(txRenderable);
    // }

    // for (const auto &rx : receivers)
    // {
    //     auto rxRenderable = std::make_shared<DeviceRenderable>(rx, glm::vec3(0.0f, 0.0f, 1.0f)); // mavi
    //     viewer.addRenderable(rxRenderable);
    // }

    // // auto cylinderRenderable = std::make_shared<PointCloudRenderable>(meshCy);
    // // cylinderRenderable->setColor(glm::vec3(1.0f, 0.5f, 0.2f));
    // // cylinderRenderable->setVisible(true);
    // // viewer.addRenderable(cylinderRenderable);

    // auto cubeWireframeLines = cube->wireframe(); // returns std::vector<Point>
    // auto renderable = std::make_shared<WireframeRenderable>(cubeWireframeLines, glm::vec3(0.6f, 0.6f, 0.6f));
    // // viewer.addRenderable(renderable);

    // // auto cylinderWireframeLines = cylinder->wireframe(); // returns std::vector<Point>
    // // renderable = std::make_shared<WireframeRenderable>(cylinderWireframeLines, glm::vec3(0.6f, 0.6f, 0.6f));
    // // viewer.addRenderable(renderable);

    // auto scene = std::make_shared<SimulationScene>();

    // scene->addTransmitters(transmitters);
    // scene->addReceivers(receivers);
    // scene->addShape(cube);
    // // scene->addShape(cylinder);

    // // SignalSolver solver(scene);
    // // auto closestPoints = solver.solve();
    // // std::cout << closestPoints->toString() << std::endl;

    // // auto closestPointRenderable = std::make_shared<PointCloudRenderable>(closestPoints);
    // // closestPointRenderable->setColor(glm::vec3(1.0f, 0.0f, 0.5f));
    // // closestPointRenderable->setVisible(true);
    // // viewer.addRenderable(closestPointRenderable);

    // // VIEWER START
    viewer::OpenGLViewer viewer(1280, 720, "ADSIL Analyzer - OpenGL");

    auto axisEntity = std::make_shared<viewer::AxisEntity>();
    viewer.addEntity(axisEntity); // Assuming `addEntity` works on IEntity*

    auto groundEntity = std::make_shared<viewer::GroundEntity>();
    viewer.addEntity(groundEntity); // Assuming `addEntity` works on IEntity*

    auto carEntity = std::make_shared<viewer::CarEntity>(carAdapter->fromJson(car_json), glm::vec3(0.2f, 0.6f, 0.9f));
    viewer.addEntity(carEntity);

    auto scene = sceneAdapter->fromJson(objects_json);
    std::cout << "[INFO] Scene loaded." << std::endl;

    for (auto shape : scene->getShapes())
    {
        auto shapeEntity = std::make_shared<viewer::ShapeEntity>(shape);
        viewer.addEntity(shapeEntity);
    }
    std::cout << "[INFO] Entities loaded." << std::endl;

    viewer.run();

    return 0;
}