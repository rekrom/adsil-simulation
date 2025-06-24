#include "main.hpp"
#include <iostream>

int main()
{
    // adapter initializations
    adapter::JsonAdapterRegistry registry;
    // register the carjson into the adapter
    registry.registerAdapter<Car>(std::make_shared<adapter::CarJsonAdapter>());

    nlohmann::json car_json;
    if (!utils::readJsonFromFile("bin/resources/car.json", car_json))
    {
        exit(0);
    }

    auto *carAdapter = registry.getAdapter<Car>();
    if (!carAdapter)
    {
        std::cerr << "Car adapter bulunamadı!\n";
        return 1;
    }

    // JSON'dan Car objesi oluştur
    Car loadedCar = carAdapter->fromJson(car_json);

    nlohmann::json j = carAdapter->toJson(loadedCar);
    std::cout << "Car: " << j.dump(4) << std::endl;

    // auto cube = ShapeFactory::createCube({Point(-300.0f / 1000.0, 1500.0f / 1000.0f, 0),
    //                                       600.0 / 1000.0f,
    //                                       Vector(0, 0, 0)});

    // // auto cylinder = ShapeFactory::createCylinder({Point(-500, 100, 0),
    // //                                               2000.0,
    // //                                               300.0,
    // //                                               Vector(0, 0, 0)});

    // // std::cout << cube->toString() << "\n";
    // // std::cout << cylinder->toString() << "\n";

    // auto mesh = cube->surfaceMesh(2048 * 16);
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
    // auto cubeRenderable = std::make_shared<PointCloudRenderable>(mesh);
    // cubeRenderable->setColor(glm::vec3(0.2f, 0.8f, 1.0f)); // isteğe göre
    // cubeRenderable->setVisible(true);                      // sadece test için görünür yap
    // viewer.addRenderable(cubeRenderable);

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
    OpenGLViewer viewer(1280, 720, "ADSIL Analyzer - OpenGL");
    // auto axis = std::make_shared<AxisRenderable>();
    // viewer.addRenderable(axis);
    viewer.run();

    return 0;
}