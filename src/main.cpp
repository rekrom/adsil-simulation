#include "main.hpp"

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
        Point(-0.4f, 0.0f, 0.0f),
        Point(-0.2f, 0.0f, 0.0f),
        Point(+0.2f, 0.0f, 0.0f),
        Point(+0.4f, 0.0f, 0.0f)};

    // Create transmitters
    auto transmitters = DeviceFactory::createTransmitters(
        tx_origins,
        transmitter_dirs,
        std::vector<float>{20, 20, 20, 20}, // vertical FOVs
        std::vector<float>{30, 30, 30, 30}  // horizontal FOVs
    );

    // Receiver setup
    std::vector<Point> rx_origins = {
        Point(-1000 / 1000, 0, 0),
        Point(500 / 1000, 500 / 1000, 1000 / 1000),
        Point(-500 / 1000, 500 / 1000, 1000 / 1000),
        Point(1000 / 1000, 0, 0)};

    std::vector<Vector> rx_dirs(4, Vector(0, 1, 0));
    std::vector<float> v_fovs(4, 20);
    std::vector<float> h_fovs(4, 120);

    auto receivers = DeviceFactory::createReceivers(
        rx_origins,
        rx_dirs,
        v_fovs,
        h_fovs);

    auto cube = ShapeFactory::createCube({Point(0, 0.5, 0),
                                          200.0f / 1000.0f,
                                          Vector(0, 0, 0)});

    auto cylinder = ShapeFactory::createCylinder({Point(0.2, 0.8, 0),
                                                  300.0f / 1000.0f,
                                                  50.0f / 1000.0f,
                                                  Vector(0, 0, M_PI / 4)});

    // std::cout << cube->toString() << "\n";
    // std::cout << cylinder->toString() << "\n";

    auto mesh = cube->surfaceMesh(25);
    // std::cout << mesh->toString() << "\n";

    auto meshCy = cylinder->surfaceMesh(25);
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

    auto cylinderRenderable = std::make_shared<PointCloudRenderable>(meshCy);
    cylinderRenderable->setColor(glm::vec3(1.0f, 0.5f, 0.2f));
    cylinderRenderable->setVisible(true);
    viewer.addRenderable(cubeRenderable);
    viewer.addRenderable(cylinderRenderable);

    auto cubeWireframeLines = cube->wireframe(); // returns std::vector<Point>
    auto renderable = std::make_shared<WireframeRenderable>(cubeWireframeLines, glm::vec3(0.6f, 0.6f, 0.6f));
    viewer.addRenderable(renderable);

    auto cylinderWireframeLines = cylinder->wireframe(); // returns std::vector<Point>
    renderable = std::make_shared<WireframeRenderable>(cylinderWireframeLines, glm::vec3(0.6f, 0.6f, 0.6f));
    viewer.addRenderable(renderable);

    auto scene = std::make_shared<SimulationScene>();
    scene->addTransmitters(transmitters);
    scene->addReceivers(receivers);
    scene->addShape(cube);
    scene->addShape(cylinder);

    SignalSolver solver(scene);
    auto closestPoints = solver.solve();
    std::cout << closestPoints->toString() << std::endl;
    std::cout << "after solving" << std::endl;
    auto closestPointRenderable = std::make_shared<PointCloudRenderable>(closestPoints);
    closestPointRenderable->setColor(glm::vec3(1.0f, 0.0f, 0.5f));
    closestPointRenderable->setVisible(true);
    viewer.addRenderable(closestPointRenderable);

    viewer.run();
    return 0;
}