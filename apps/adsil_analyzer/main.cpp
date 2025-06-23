#include "main.hpp"
#include <iostream>

int main()
{
    // Define transmitter angles and directions
    std::vector<float> transmitter_angles = {-45.0f, -15.0f, 15.0f, 45.0f};
    std::vector<Vector> transmitter_dirs;

    for (float angle_deg : transmitter_angles)
    {
        float angle_rad = angle_deg * static_cast<float>(M_PI) / 180.0f;
        transmitter_dirs.emplace_back(std::sin(angle_rad), std::cos(angle_rad), 0.0f);
    }

    // Origins for transmitters (same point)
    std::vector<Point> tx_origins = {
        Point(0.0f, 0.0f, 0.0f),
        Point(0.0f, 0.0f, 0.0f),
        Point(0.0f, 0.0f, 0.0f),
        Point(0.0f, 0.0f, 0.0f)};

    // Create transmitters
    auto transmitters = DeviceFactory::createTransmitters(
        tx_origins,
        transmitter_dirs,
        std::vector<float>{20, 20, 20, 20}, // vertical FOVs
        std::vector<float>{30, 30, 30, 30}  // horizontal FOVs
    );

    // Receiver setup
    std::vector<Point> rx_origins = {
        Point(-1000 / 1000.0f, 0, 0),
        Point(1000 / 1000.0f, 0, 0)};

    std::vector<Vector> rx_dirs(rx_origins.size(), Vector(0, 1, 0));
    std::vector<float> v_fovs(rx_origins.size(), 20);
    std::vector<float> h_fovs(rx_origins.size(), 120);

    auto receivers = DeviceFactory::createReceivers(
        rx_origins,
        rx_dirs,
        v_fovs,
        h_fovs);

    auto cube = ShapeFactory::createCube({Point(-300.0f / 1000.0, 1500.0f / 1000.0f, 0),
                                          600.0 / 1000.0f,
                                          Vector(0, 0, 0)});

    // auto cylinder = ShapeFactory::createCylinder({Point(-500, 100, 0),
    //                                               2000.0,
    //                                               300.0,
    //                                               Vector(0, 0, 0)});

    // std::cout << cube->toString() << "\n";
    // std::cout << cylinder->toString() << "\n";

    auto mesh = cube->surfaceMesh(2048 * 16);
    std::cout << mesh->toString() << "\n";

    // auto meshCy = cylinder->surfaceMesh(16);
    // std::cout << meshCy->toString() << "\n";

    // VIEWER START
    OpenGLViewer viewer(1280, 720, "ADSIL Analyzer - OpenGL");
    auto axis = std::make_shared<AxisRenderable>();
    viewer.addRenderable(axis);
    for (const auto &tx : transmitters)
    {
        auto txRenderable = std::make_shared<DeviceRenderable>(tx, glm::vec3(1.0f, 0.0f, 0.0f)); // kırmızı
        viewer.addRenderable(txRenderable);
    }

    for (const auto &rx : receivers)
    {
        auto rxRenderable = std::make_shared<DeviceRenderable>(rx, glm::vec3(0.0f, 0.0f, 1.0f)); // mavi
        viewer.addRenderable(rxRenderable);
    }
    auto cubeRenderable = std::make_shared<PointCloudRenderable>(mesh);
    cubeRenderable->setColor(glm::vec3(0.2f, 0.8f, 1.0f)); // isteğe göre
    cubeRenderable->setVisible(true);                      // sadece test için görünür yap
    viewer.addRenderable(cubeRenderable);

    // auto cylinderRenderable = std::make_shared<PointCloudRenderable>(meshCy);
    // cylinderRenderable->setColor(glm::vec3(1.0f, 0.5f, 0.2f));
    // cylinderRenderable->setVisible(true);
    // viewer.addRenderable(cylinderRenderable);

    auto cubeWireframeLines = cube->wireframe(); // returns std::vector<Point>
    auto renderable = std::make_shared<WireframeRenderable>(cubeWireframeLines, glm::vec3(0.6f, 0.6f, 0.6f));
    // viewer.addRenderable(renderable);

    // auto cylinderWireframeLines = cylinder->wireframe(); // returns std::vector<Point>
    // renderable = std::make_shared<WireframeRenderable>(cylinderWireframeLines, glm::vec3(0.6f, 0.6f, 0.6f));
    // viewer.addRenderable(renderable);

    auto scene = std::make_shared<SimulationScene>();

    scene->addTransmitters(transmitters);
    scene->addReceivers(receivers);
    scene->addShape(cube);
    // scene->addShape(cylinder);

    SignalSolver solver(scene);
    auto closestPoints = solver.solve();
    std::cout << closestPoints->toString() << std::endl;

    auto closestPointRenderable = std::make_shared<PointCloudRenderable>(closestPoints);
    closestPointRenderable->setColor(glm::vec3(1.0f, 0.0f, 0.5f));
    closestPointRenderable->setVisible(true);
    viewer.addRenderable(closestPointRenderable);

    Point origin(0, 0, 0);
    Vector orientation(0, 0, 0);
    std::vector<std::shared_ptr<Device>> tx;
    std::vector<std::shared_ptr<Device>> rx;

    tx.clear();
    rx.clear();

    Car car(origin, orientation, tx, rx);

    nlohmann::json j = car.toJson();
    std::cout << j.dump(4) << std::endl;

    JsonUtils::saveToFile("car.json", j);
    std::cout << "done" << std::endl;

    Car loadedCar;

    // viewer.run();
    return 0;
}