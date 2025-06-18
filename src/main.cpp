#include "DeviceFactory.hpp"
#include "ShapeFactory.hpp"
#include "Point.hpp"
#include "Vector.hpp"
#include <iostream>
#include <cmath>
#include "Viewer/OpenGLViewer.hpp"
#include "Viewer/AxisRenderable.hpp"
#include "Viewer/DeviceRenderable.hpp"

using std::vector;

int main()
{
    // Define transmitter angles and directions
    vector<float> transmitter_angles = {-45.0f, -15.0f, 15.0f, 45.0f};
    vector<Vector> transmitter_dirs;

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
        vector<float>{20, 20, 20, 20}, // vertical FOVs
        vector<float>{30, 30, 30, 30}  // horizontal FOVs
    );

    // Receiver setup
    vector<Point> rx_origins = {
        Point(-1000, 0, 0),
        Point(500, 500, 1000),
        Point(-500, 500, 1000),
        Point(1000, 0, 0)};

    vector<Vector> rx_dirs(4, Vector(0, 1, 0));
    vector<float> v_fovs(4, 20);
    vector<float> h_fovs(4, 120);

    auto receivers = DeviceFactory::createReceivers(
        rx_origins,
        rx_dirs,
        v_fovs,
        h_fovs);

    auto cube = ShapeFactory::createCube({Point(0, 0, 0),
                                          200.0f,
                                          Vector(0, 0, 0)});

    auto cylinder = ShapeFactory::createCylinder({Point(100, 0, 0),
                                                  300.0f,
                                                  50.0f,
                                                  Vector(0, 0, M_PI / 4)});

    std::cout << cube->toString() << "\n";
    std::cout << cylinder->toString() << "\n";

    auto mesh = cube->surfaceMesh(25);
    std::cout << mesh->toString() << "\n";

    auto meshCy = cylinder->surfaceMesh(25);
    std::cout << meshCy->toString() << "\n";

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

    viewer.run();
    return 0;
}